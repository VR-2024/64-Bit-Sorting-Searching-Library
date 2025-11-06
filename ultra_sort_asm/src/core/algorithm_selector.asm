; src/core/algorithm_selector.asm
; Implements the "brain" of the sort library (Patent Claim 3).
; This function analyzes the data and selects the optimal
; sort algorithm to call.

section .rodata
    ; Define entropy thresholds for comparison
    align 8
    entropy_high: dq 0.85
    align 8
    entropy_low:  dq 0.3

section .text
    ; External functions (our other assembly files)
    extern calculate_entropy
    extern insertion_sort_asm
    extern radix_sort_asm
    extern bitonic_sort_avx512

    global select_and_sort

; void select_and_sort(int64_t* array, uint64_t count)
; RDI: array (int64_t*)
; RSI: count (uint64_t)
select_and_sort:
    push    rbp
    mov     rbp, rsp
    ; Save all callee-saved registers we will modify
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15

    mov     r12, rdi    ; r12 = array
    mov     r13, rsi    ; r13 = count
    
    ; --- 1. Check for small arrays ---
    ; If count <= 64, use insertion sort (it's fastest)
    cmp     r13, 64
    jle     .use_insertion_sort

    ; --- 2. Check for AVX-512 Support (for Bitonic Path) ---
    ; We do this *before* entropy to save the result.
    xor     r15, r15    ; r15 = 0 (is_avx512_supported = false)
    mov     eax, 7
    xor     ecx, ecx
    cpuid               ; CPUID, leaf 7
    test    ebx, (1 << 16) ; Check EBX bit 16 for AVX-512F
    jz      .skip_avx_flag ; If zero, not supported, r15 remains 0
    mov     r15, 1      ; If not zero, set r15 = 1 (true)
.skip_avx_flag:

    ; --- 3. Calculate Entropy ---
    ; call calculate_entropy(array, count)
    ; Note: RDI and RSI are still array/count from the original call
    call    calculate_entropy
    ; Result is now in XMM0

    ; --- 4. Decision Tree ---

    ; Check 4a: if (entropy > 0.85) -> Use Radix Sort
    movsd   xmm1, [rel entropy_high]
    ucomisd xmm0, xmm1  ; Compare xmm0 with 0.85
    ja      .use_radix_sort ; Jump if above (unsigned float compare)

    ; Check 4b: if (entropy < 0.3) -> Use Insertion Sort
    movsd   xmm1, [rel entropy_low]
    ucomisd xmm0, xmm1  ; Compare xmm0 with 0.3
    jb      .use_insertion_sort ; Jump if below

    ; Check 4c: Medium Entropy (0.3 - 0.85)
    ; This is the path for Bitonic or Radix fallback.
    
    ; Check 4c-1: Is count a power of 2?
    mov     rax, r13    ; rax = count
    dec     rax         ; rax = count - 1
    and     rax, r13    ; rax = (count - 1) & count
    test    rax, rax    ; If result is 0, it's a power of 2
    jnz     .use_radix_sort ; Not a power of 2, use Radix

    ; Check 4c-2: Is AVX-512 supported?
    cmp     r15, 1      ; Compare r15 (our saved flag) with 1
    jne     .use_radix_sort ; Not supported, use Radix
    
    ; If we are here: Medium Entropy AND Power of 2 AND AVX-512 is supported
    jmp     .use_bitonic_sort

    ; --- 5. Call Selected Algorithm ---

.use_insertion_sort:
    mov     rdi, r12    ; Set arg1 = array
    mov     rsi, r13    ; Set arg2 = count
    call    insertion_sort_asm
    jmp     .done

.use_radix_sort:
    mov     rdi, r12    ; Set arg1 = array
    mov     rsi, r13    ; Set arg2 = count
    call    radix_sort_asm
    jmp     .done

.use_bitonic_sort:
    mov     rdi, r12    ; Set arg1 = array
    mov     rsi, r13    ; Set arg2 = count
    call    bitonic_sort_avx512
    jmp     .done

.done:
    ; Restore registers
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    pop     rbp
    ret