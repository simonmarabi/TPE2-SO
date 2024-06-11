GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL _cli
GLOBAL _sti

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq10Handler
GLOBAL _irq80Handler

GLOBAL _invalidOpCodeInterruption
GLOBAL _divisionByZeroInterruption

GLOBAL _int20

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscallDispatcher
EXTERN scheduler

GLOBAL info
;GLOBAL screenshot

SECTION .text
										; Macros que se utilizan para guardar y restaurar el estado de los registros en la pila.
%macro pushState 0						
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro
										; Macros que se utilizan para guardar y restaurar el estado de los registros en la pila.
%macro popState 0						
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro
										; Macros que se utilizan para guardar y restaurar el estado de los registros en la pila.
%macro pushStateNoRax 0					
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro
										; Macros que se utilizan para guardar y restaurar el estado de los registros en la pila.
%macro popStateNoRax 0					
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro irqHandlerMaster 1
	cli
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	sti
	iretq
%endmacro

%macro exceptionHandler 1
	; opcion que trae el valor de RIP cuando ocurre la excepcion tomando la direccion de retorno de la interrupcion
	mov [regdata + (1*8)], rax	;rax
	mov rax, [rsp]
	mov [regdata], rax			;rip
	pushState
	
	;me guardo los registros para imprimir
	;Guardo: rip, rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15, rflags

	mov [regdata + (2*8)], rbx	;rbx
	mov [regdata + (3*8)], rcx	;rbx
	mov [regdata + (4*8)], rdx	;rdx
	mov [regdata + (5*8)], rsi	;rsi
	mov [regdata + (6*8)], rdi	;rdi
	mov [regdata + (7*8)], rbp	;rbp
	mov rax, rsp ; We get the value of RSP 
	add rax, 0x28 ; We add bytes in order to compensate for the values pushed since the exception occurred and called the exception handler
	mov [regdata + (8*8)], rax	;rsp
	mov [regdata + (9*8)], r8	;r8
	mov [regdata + (10*8)], r9	;r9
	mov [regdata + (11*8)], r10	;r10
	mov [regdata + (12*8)], r11	;r11
	mov [regdata + (13*8)], r12	;r12
	mov [regdata + (14*8)], r13	;r13
	mov [regdata + (15*8)], r14	;r14
	mov [regdata + (16*8)], r15	;r15
	mov rax, [rsp+8] ; We get the value of RFLAGS (it is pushed when an interrupt occurs).
	mov [regdata + (17*8)], rax	;rflags

	mov rdi, %1 ; pasaje de parametro
	mov rsi, regdata
	call exceptionDispatcher

	popState
	iretq
%endmacro

_int20:
	int 20h
	ret

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push	rbp
    mov rbp, rsp   
    mov ax, di
    out	21h,al
    pop		rbp
    retn

picSlaveMask:
	push	rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out		0A1h, al
    pop     rbp
    retn

;8254 Timer (Timer Tick)
_irq00Handler:
	pushState

	mov rdi, 0 ; pasaje de parametro
	call irqDispatcher

	mov rdi, rsp
	call scheduler
	mov rsp, rax

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq

;Keyboard
_irq01Handler: irqHandlerMaster 1
	
;Cascade pic never called
_irq02Handler: irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler: irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler: irqHandlerMaster 4

;USB
_irq05Handler: irqHandlerMaster 5

;Syscall
_irq80Handler:
	pushStateNoRax
	;Syscall params: rdi rsi rdx r10 r8	r9
	;C params: rdi rsi rdx rcx r8 r9

	mov rcx, r10
	mov r9, rax
	call syscallDispatcher
	
	popStateNoRax
	iretq

;Zero Division Exception
_divisionByZeroInterruption:
	exceptionHandler 00h

;Invalid Op Code Exception
_invalidOpCodeInterruption:
	exceptionHandler 06h

haltcpu:
	cli
	hlt
	ret

SECTION .bss
	aux resq 1
	info resq 17			; reservo 17 espacios de 8 bytes para guardar registros en caso de screenshot
	regdata resq 18			; reservo 18 espacios de 8 bytes para guardar registros en caso de excepción
	;screenshot resb 1 		; reservo un byte para poner en 1 si hubo un screenshot
