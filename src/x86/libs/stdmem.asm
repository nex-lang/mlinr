section .bss
    __heap_start resb 1024 * 1024
    __heap_end   resq 1
    __heap_free  resq 1

section .text
global __malloc, __free, __sbrk

__sbrk:
    test rdi, rdi
    jz __sbrk_zero

    mov rax, [__heap_free]
    add rax, rdi
    mov rbx, [__heap_end]
    cmp rax, rbx
    jg __sbrk_fail

    mov [__heap_free], rax
    ret

__sbrk_zero:
    mov rax, [__heap_start]
    ret

__sbrk_fail:
    mov rax, -1
    ret


__heap:
    mov rax, __heap_start
    mov [__heap_free], rax
    add rax, 1024 * 1024
    mov [__heap_end], rax
    ret

__malloc:
    add rdi, 15
    and rdi, -16
    mov rax, [__heap_free]
    add rax, rdi
    cmp rax, [__heap_end]
    jbe ___malloc_alloc
    mov rax, rdi
    call __sbrk
    test rax, rax
    js __malloc_fail
    mov rbx, rax
    mov [__heap_end], rbx
    add rbx, rdi
    mov [__heap_free], rbx

___malloc_alloc:
    mov rax, [__heap_free]
    add [__heap_free], rdi
    ret

__malloc_fail:
    xor rax, rax
    ret

__free:
    mov rax, [__heap_free]
    cmp rdi, rax
    ja __free_fail 

    mov rax, rdi
    mov [__heap_free], rax
    ret

__free_fail:
    ret