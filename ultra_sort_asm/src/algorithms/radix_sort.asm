; src/algorithms/radix_sort.asm
; Implements a 64-bit SIGNED (int64_t) radix sort.
;
; Fixes the signed sort bug by XORing all numbers with 0x8000...
; to map them to a sortable unsigned range, and then XORing
; them back after the sort.

section .rodata
    align 8
    ; Constant used to flip the MSB (2^63)
    xor_flip_const: dq 0x8000000000000000

section .bss
    ; Temporary array for one pass (1M 64-bit elements)
    temp_array: resq 1000000
    ; Histogram for current pass (256 32-bit counts)
    histogram: resd 256

section .text
global radix_sort_asm

radix_sort_asm:
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15
    push    rdi
    push    rsi
    push    r8
    push    r9

    mov     r12, rdi    ; r12 = array
    mov     r13, rsi    ; r13 = count
    
    ; --- 0. Map signed to unsigned (XOR MSB) ---
    xor     r9, r9      ; r9 = i = 0
    mov     rbx, [xor_flip_const] ; rbx = 0x8000...
.map_loop_fwd:
    cmp     r9, r13
    jge     .map_loop_fwd_done
    xor     qword [r12 + r9*8], rbx ; array[i] ^= 0x8000...
    inc     r9
    jmp     .map_loop_fwd
.map_loop_fwd_done:

    ; --- Begin Original Radix Sort Logic ---
    xor     r14, r14    ; r14 = current pass number (0 to 7)
.pass_loop:
    cmp     r14, 8
    jge     .sort_done

    mov     r15, r14
    shl     r15, 3      ; r15 = r14 * 8 (bit shift)
    mov     cl, r15b    ; cl = shift amount (0, 8, 16...)

    ; --- 1. Build Histogram ---
    mov     rdi, histogram
    xor     rax, rax
    mov     rcx, 128    ; 256 dwords = 128 qwords
    rep     stosq       ; Clear histogram

    xor     r9, r9      ; r9 = i = 0 (loop counter)
.histogram_loop:
    cmp     r9, r13     ; i < count
    jge     .histogram_done

    mov     rax, [r12 + r9*8] ; rax = array[i]
    
    ; Must reset CL, as 'rep stosq' modifies rcx/cl
    mov     cl, r15b    
    shr     rax, cl     ; Shift by cl
    and     rax, 0xFF   ; Extract 8-bit digit
    
    inc     dword [histogram + rax*4] ; Increment histogram[digit]

    inc     r9          ; i++
    jmp     .histogram_loop
.histogram_done:

    ; --- 2. Prefix Sum (Correct "End-of-Bucket" logic) ---
    xor     rcx, rcx    ; rcx = i = 0
    xor     rax, rax    ; rax = total_sum = 0
.prefix_loop:
    cmp     rcx, 256
    jge     .prefix_done

    mov     ebx, [histogram + rcx*4] ; ebx = count[i]
    add     rax, rbx                 ; total_sum += count[i]
    mov     [histogram + rcx*4], eax ; hist[i] = total_sum (end index)
    
    inc     rcx
    jmp     .prefix_loop
.prefix_done:

    ; --- 3. Scatter elements to temp array (Correct Stable Scatter) ---
    mov     r8, histogram ; r8 = base address of histogram
    mov     cl, r15b      ; cl = shift amount

    ; Iterate *backwards* for stability
    mov     r9, r13     ; r9 = i = count
.scatter_loop:
    dec     r9          ; i-- (from count-1 down to 0)
    cmp     r9, -1
    je      .scatter_done

    mov     rax, [r12 + r9*8] ; rax = element = array[i]

    ; Calculate digit
    mov     rbx, rax
    shr     rbx, cl     ; Shift by cl
    and     rbx, 0xFF   ; rbx = digit (0-255)
    
    dec     dword [r8 + rbx*4]
    mov     edi, [r8 + rbx*4] ; edi = index
    mov     [temp_array + rdi*8], rax ; temp_array[index] = element
    
    jmp     .scatter_loop
.scatter_done:

    ; --- 4. Copy temp back to original ---
    mov     rsi, temp_array
    mov     rdi, r12
    mov     rcx, r13
    rep     movsq

    inc     r14           ; Next pass
    jmp     .pass_loop

.sort_done:
    ; --- 5. Map unsigned back to signed (XOR MSB) ---
    xor     r9, r9      ; r9 = i = 0
    mov     rbx, [xor_flip_const] ; rbx = 0x8000...
.map_loop_rev:
    cmp     r9, r13
    jge     .map_loop_rev_done
    xor     qword [r12 + r9*8], rbx ; array[i] ^= 0x8000...
    inc     r9
    jmp     .map_loop_rev
.map_loop_rev_done:

    ; --- Epilogue ---
    pop     r9
    pop     r8
    pop     rsi
    pop     rdi
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    pop     rbp
    ret