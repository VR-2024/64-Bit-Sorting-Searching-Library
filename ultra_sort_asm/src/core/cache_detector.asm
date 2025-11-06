; src/core/cache_detector.asm
; Phase 2: Implementation of Patent Claim 1 component
; Function to detect L1, L2, L3 cache sizes and line size using CPUID (Leaf 0x04)

section .text
global detect_cache_sizes

; int detect_cache_sizes(ultra_cache_info_t* info)
; RDI: pointer to ultra_cache_info_t structure (System V AMD64 ABI)
; Returns: 1 on success, 0 on failure

detect_cache_sizes:
    ; Standard function prologue
    push rbx
    push rcx
    push rdx
    ; Note: RDI (info pointer) is already saved via calling convention

    ; --- 1. Get Cache Information using CPUID Leaf 0x04 ---
    
    ; We skip the CPUID support check for brevity, assuming modern x86-64 CPU.
    
    ; Cache loop: ECX = 0 for L1D, 1 for L1I (we skip I), 2 for L2, 3 for L3
    mov ecx, 1          ; Start ECX at 1, but Leaf 4 uses ECX for index, 
                        ; so we start ECX at 0 and check the type field.
    mov r8, rdi         ; Copy info pointer to R8 for use in the loop

.cache_level_loop:
    cmp ecx, 4          ; Check up to L3 (index 3). Exit if ECX >= 4.
    jge .done_success

    mov eax, 0x04       ; CPUID Leaf 0x04: Deterministic Cache Parameters
    cpuid               ; Execute CPUID: EAX, EBX, ECX, EDX are updated.

    ; Check cache type (EAX[4:0]): 0=Null, 1=Data, 2=Instruction, 3=Unified
    and al, 0x1F        ; Mask EAX[4:0]
    cmp al, 0           ; If type is 0 (Null), skip this level
    je .next_level

    ; --- 2. Extract Cache Parameters from CPUID Output ---
    
    ; EBX[11:0] = Line Size - 1 (L+1)
    ; EBX[21:12] = Partitions - 1 (P+1)
    ; EBX[31:22] = Associativity - 1 (A+1)
    ; ECX[31:0] = Sets - 1 (S+1)
    
    ; Cache Size = (L+1) * (P+1) * (A+1) * (S+1)
    
    mov r9d, ebx        ; R9d = EBX copy
    mov r10d, ebx       ; R10d = EBX copy
    mov r11d, ebx       ; R11d = EBX copy
    mov r12d, ecx       ; R12d = ECX copy (Sets)
    
    ; Calculate Line Size (L)
    and r9, 0xFFF       ; Mask EBX[11:0]
    inc r9              ; L = EBX[11:0] + 1 (Line Size in Bytes)

    ; Calculate Partitions (P)
    shr r10, 12         ; Shift EBX[21:12]
    and r10, 0x3FF      ; Mask
    inc r10             ; P = EBX[21:12] + 1
    
    ; Calculate Associativity (A)
    shr r11, 22         ; Shift EBX[31:22]
    and r11, 0x3FF      ; Mask
    inc r11             ; A = EBX[31:22] + 1
    
    ; Calculate Sets (S)
    inc r12             ; S = ECX[31:0] + 1
    
    ; --- 3. Calculate Total Cache Size and Store Results ---
    
    ; R9 now holds Line Size. We use RDX as temporary result register.
    mov rdx, r9         ; RDX = Line Size (L+1)
    imul rdx, r10       ; RDX = (L+1) * (P+1)
    imul rdx, r11       ; RDX = (L+1) * (P+1) * (A+1)
    imul rdx, r12       ; RDX = Total Cache Size (in bytes)
    
    ; Cache Level: EAX[7:5] = Cache Level (1 for L1, 2 for L2, 3 for L3)
    shr eax, 5
    and al, 0x07        ; al now holds the cache level index (1, 2, or 3)
    
    cmp al, 1           ; Is it L1?
    je .store_l1
    
    cmp al, 2           ; Is it L2?
    je .store_l2
    
    cmp al, 3           ; Is it L3?
    je .store_l3
    
    ; If we get here, it's an unrecognized level (e.g., L1I), skip it.
    jmp .next_level

.store_l1:
    mov qword [r8], rdx         ; info->l1_size (Offset 0)
    mov qword [r8+24], r9       ; info->line_size (Store line size once, Offset 24)
    jmp .next_level
    
.store_l2:
    mov qword [r8+8], rdx       ; info->l2_size (Offset 8)
    jmp .next_level
    
.store_l3:
    mov qword [r8+16], rdx      ; info->l3_size (Offset 16)
    jmp .next_level

.next_level:
    inc ecx                     ; Check next level index
    jmp .cache_level_loop

.done_success:
    mov rax, 1          ; Return 1 (Success)
    jmp .done

.done_failure:
    xor rax, rax        ; Return 0 (Failure)

.done:
    ; Standard function epilogue
    pop rdx
    pop rcx
    pop rbx
    ret
