; src/algorithms/insertion_sort.asm
; Implements a 64-bit insertion sort.
; Optimized for small arrays (< 64 elements) by
; keeping the "key" and loop counters in registers.
;
; void insertion_sort_asm(int64_t* array, uint64_t count)
; RDI: array (int64_t*)
; RSI: count (uint64_t)

section .text
global insertion_sort_asm

insertion_sort_asm:
    ; Standard C-style loop: for (i = 1; i < count; i++)
    ; R10 will be our outer loop counter 'i'
    mov     r10, 1          ; i = 1

.outer_loop:
    cmp     r10, rsi        ; compare i, count
    jge     .done           ; jump if i >= count

    ; key = array[i]
    ; R11 will hold the 'key' (the value we are inserting)
    mov     r11, [rdi + r10*8] ; r11 = array[i]

    ; j = i - 1
    ; R8 will be our inner loop counter 'j'
    mov     r8, r10
    sub     r8, 1           ; j = i - 1

.inner_loop:
    ; while (j >= 0 && array[j] > key)
    cmp     r8, 0           ; compare j, 0
    jl      .insert         ; jump if j < 0 (signed comparison, so -1 is < 0)

    ; R9 will temporarily hold array[j]
    mov     r9, [rdi + r8*8]  ; r9 = array[j]
    cmp     r9, r11         ; compare array[j], key
    jle     .insert         ; jump if array[j] <= key

    ; array[j + 1] = array[j]
    mov     [rdi + r8*8 + 8], r9 ; array[j+1] = r9 (which is array[j])

    ; j = j - 1
    dec     r8
    jmp     .inner_loop

.insert:
    ; array[j + 1] = key
    ; We add 8 because r8 is currently j (which might be -1)
    ; j+1 is the correct insertion point
    mov     [rdi + r8*8 + 8], r11

    ; i++
    inc     r10
    jmp     .outer_loop

.done:
    ret
