#!/bin/bash

# Para WSL:

if [[ "$1" == "gdb" ]]; then
    # Iniciar el emulador en modo de depuración con GDB
    sudo qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 
else
    # Iniciar el emulador en modo normal con sonido
    sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 
fi


# Para Mac:

# if [[ "$1" == "gdb" ]]; then
#     sudo qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
# else
#     sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
# fi

