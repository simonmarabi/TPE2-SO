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
