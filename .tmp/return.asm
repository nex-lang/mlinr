section .text
global _start

main:
	mov rbp, rsp
	and rbp, -16
	sub rsp, 0

	mov rax, 60
	syscall
