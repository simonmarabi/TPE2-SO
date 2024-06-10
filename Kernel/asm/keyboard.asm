GLOBAL readKeyRaw
GLOBAL readKeyPoll

readKeyRaw:
    mov rax,0
    in al,0x64
    and al,0x01
    cmp al,0
    jne .read
    mov rax,0
    ret
.read:
    in al,0x60
    ret

readKeyPoll:
    mov rax,0

.loop:
    in al,0x64       
    and al,0x01
    cmp al,0
    je .loop

    in al,0x60
    ret
