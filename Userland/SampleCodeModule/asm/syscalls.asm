GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_time
GLOBAL sys_inforeg
GLOBAL sys_changeFontSize
GLOBAL sys_printColor
GLOBAL sys_clear_screen
GLOBAL sys_getScreenData
GLOBAL sys_paint_rect
GLOBAL sys_getTicks
GLOBAL sys_timed_read
GLOBAL sys_beeper
GLOBAL sys_printMem
GLOBAL sys_alloc
GLOBAL sys_free
GLOBAL sys_exit
GLOBAL sys_getpid
GLOBAL sys_kill
GLOBAL sys_chgpriority
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_yield
GLOBAL sys_pipe
GLOBAL sys_close
GLOBAL sys_mkfifo
GLOBAL sys_openfifo
GLOBAL sys_setbackground
GLOBAL sys_mapstdfds
GLOBAL sys_rmfifo
GLOBAL sys_listpipes
GLOBAL sys_listprocesses
GLOBAL sys_semopen
GLOBAL sys_semwait
GLOBAL sys_sempost
GLOBAL sys_semclose
GLOBAL sys_listsem
GLOBAL sys_createprocess

section .text
sys_read:
    mov rax, 0x00       ; Número de función para sys_read
    int 80h             ; Realizar la llamada al sistema
    ret

sys_write:
    mov rax, 0x01       ; Número de función para sys_write
    int 80h             ; Realizar la llamada al sistema
    ret

sys_time:
    mov rax, 0x02       ; Número de función para sys_time
    int 80h             ; Realizar la llamada al sistema
    ret

sys_inforeg:
    mov rax, 0x03       ; Número de función para sys_inforeg
    int 80h             ; Realizar la llamada al sistema
    ret

sys_changeFontSize:
    mov rax, 0x04       ; Número de función para sys_changeFontSize
    int 80h             ; Realizar la llamada al sistema
    ret

sys_printColor:
    mov rax, 0x05       ; Número de función para sys_printColor
    int 80h             ; Realizar la llamada al sistema
    ret

sys_clear_screen:
    mov rax, 0x06       ; Número de función para sys_clear_screen
    int 80h             ; Realizar la llamada al sistema
    ret

sys_getScreenData:
    mov rax, 0x07       ; Número de función para sys_getScreenData
    int 80h             ; Realizar la llamada al sistema
    ret 

sys_paint_rect:
    mov rax, 0x08       ; Número de función para sys_paint_rect
    mov r10, rcx        
    int 80h             ; Realizar la llamada al sistema
    ret

sys_getTicks:
    mov rax, 0x09       ; Número de función para sys_getTicks
    int 80h             ; Realizar la llamada al sistema
    ret

sys_beeper:
    mov rax, 0x0A       ; Número de función para sys_beeper
    int 80h             ; Realizar la llamada al sistema
    ret

sys_printMem:
    mov rax, 0x0B       ; Número de función para sys_printMem
    int 80h             ; Realizar la llamada al sistema
    ret

sys_alloc:
    mov rax, 0x0C       ; Número de función para sys_alloc
    int 80h             ; Realizar la llamada al sistema
    ret

sys_free:
    mov rax, 0x0D       ; Número de función para sys_free
    int 80h             ; Realizar la llamada al sistema
    ret

sys_exit:
    mov rax, 0x0E       
    int 80h             
    ret 

sys_getpid:
    mov rax, 0x0F
    int 80h
    ret   

sys_kill:
    mov rax, 0x10
    int 80h
    ret

sys_chgpriority:
    mov rax, 0x11
    int 80h
    ret

sys_block:
    mov rax, 0x12
    int 80h
    ret

sys_unblock:
    mov rax, 0x13
    int 80h
    ret

sys_yield:
    mov rax, 0x14
    int 80h
    ret

sys_pipe:
    mov rax, 0x15
    int 80h
    ret

sys_close:
    mov rax, 0x16
    int 80h
    ret

sys_mkfifo:
    mov rax, 0x17       
    int 80h             
    ret 

sys_openfifo:
    mov rax, 0x18       
    int 80h             
    ret         

sys_setbackground:
    mov rax, 0x19       
    int 80h             
    ret 

sys_mapstdfds:
    mov rax, 0x1A       
    int 80h             
    ret 

sys_rmfifo:
    mov rax, 0x1B       
    int 80h             
    ret 

sys_listpipes:
    mov rax, 0x1C       
    int 80h             
    ret 

sys_listprocesses:
    mov rax, 0x1D       
    int 80h             
    ret 

sys_semopen:
    mov rax, 0x1E       
    int 80h             
    ret 

sys_semwait:
    mov rax, 0x1F       
    int 80h             
    ret 

sys_sempost:
    mov rax, 0x20       
    int 80h             
    ret 

sys_semclose:
    mov rax, 0x21       
    int 80h             
    ret 

sys_listsem:
    mov rax, 0x22       
    int 80h             
    ret 

sys_createprocess:
    mov rax, 0x23       
    int 80h             
    ret 
