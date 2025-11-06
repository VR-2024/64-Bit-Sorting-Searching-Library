; src/core/data_analyzer.asm
; Phase 2: Implementation of Patent Claim 3 component
; Fast entropy calculation using POPCNT instruction.

section .text
global calculate_entropy

; double calculate_entropy(int64_t* array, uint64_t count)
; RDI: array pointer (int64_t*)
; RSI: count (uint64_t)
; Returns: normalized entropy value in XMM0 (0.0 - 1.0)

calculate_entropy:
    ; Save caller-saved registers used by this function
    push rbx
    push r12
    push r13
    push r14
    
    ; RDI (array) and RSI (count) are arguments
    mov r12, rdi        ; R12 = array pointer
    mov r13, rsi        ; R13 = count
    
    ; --- 1. Determine Sample Size and Step ---
    
    mov r14, 4096       ; R14 = Desired sample size (4096 elements)
    cmp r13, r14        ; Compare total count with 4096
    cmovb r14, r13      ; R14 = min(count, 4096) - Actual sample size

    xor rdx, rdx        ; Clear RDX for division
    mov rax, r13        ; RAX = count
    div r14             ; RAX = count / sample_size (Sampling Step)
    mov r10, rax        ; R10 = Sampling Step (r13 elements skipped per sample)

    ; --- 2. Calculate Total Bit Count (using POPCNT) ---
    
    mov r11, 0          ; R11 = Total bit count accumulator (64-bit)
    mov rcx, 0          ; RCX = Sample loop counter (0 to R14 - 1)
    
.sample_loop:
    cmp rcx, r14        ; Check if loop counter reached sample size
    jge .calculate      ; Done sampling

    ; Get element index: index = RCX * Sampling_Step
    mov rax, r10        ; RAX = Sampling Step
    imul rax, rcx       ; RAX = Index in the original array
    
    ; Load the 64-bit element at R12 + RAX*8
    mov rbx, [r12 + rax*8] ; RBX = 64-bit element
    
    ; POPCNT: Count set bits in RBX and store result in RBX
    popcnt rbx, rbx     ; Hardware-accelerated count of set bits
    
    add r11, rbx        ; Accumulate total set bits in R11
    
    inc rcx             ; Increment sample loop counter
    jmp .sample_loop

    ; --- 3. Normalize Result (Floating Point Calculation) ---
.calculate:
    ; Expected Max Bits = Sample Size * 64
    mov rax, r14        ; RAX = Sample Size
    shl rax, 6          ; RAX = Sample Size * 64 (Max possible set bits)
    
    ; Convert R11 (bit_count) to double in XMM0
    cvtsi2sd xmm0, r11  ; XMM0 = (double) Total_Bit_Count
    
    ; Convert RAX (total_bits) to double in XMM1
    cvtsi2sd xmm1, rax  ; XMM1 = (double) Max_Possible_Bits
    
    ; Calculate Entropy = Total_Bit_Count / Max_Possible_Bits
    divsd xmm0, xmm1    ; XMM0 = XMM0 / XMM1 (Normalized Entropy)
    ; XMM0 now holds the return value (0.0 - 1.0)
    
    ; Restore caller-saved registers
    pop r14
    pop r13
    pop r12
    pop rbx
    
    ; XMM0 is the return value for floating point functions
    ret
