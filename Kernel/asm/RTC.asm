GLOBAL _NRTCGetHours
GLOBAL _NRTCGetMins
GLOBAL _NRTCGetSeconds

section .text

; Función para obtener las horas del RTC
_NRTCGetHours:
    mov rax, 0x04
    call _RTC

    ret

; Función para obtener los minutos del RTC
_NRTCGetMins:
    mov rax, 0x02
    call _RTC
    
    ret

; Función para obtener los segundos del RTC
_NRTCGetSeconds:
    mov rax, 0x00
    call _RTC

    ret

; Función que realiza la operación de lectura de RTC 
_RTC:
    out 70h, al
    in al, 71h
    ret
