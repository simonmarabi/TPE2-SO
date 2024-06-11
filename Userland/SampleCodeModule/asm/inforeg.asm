GLOBAL inforeg_asm

EXTERN inforeg_print

SECTION .text

inforeg_asm:
    push rbp
    mov rbp,rsp

    ;Armo el struct en stack para poder accederlo desde C
	pushf
    push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rsi
	push rdi
	push rdx
	push rcx
	push rbx
	push rax

    mov rax,rbp
	add rax,16
    push rax

    mov rdi, rsp
    call inforeg_print

    ;Desarmo el stack
    add rsp, 8
    pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	popf

    leave
    ret
