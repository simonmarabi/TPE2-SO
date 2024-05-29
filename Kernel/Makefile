include Makefile.inc

KERNEL=kernel.bin
SOURCES= $(wildcard *.c) $(wildcard MemoryManager/*.c) $(wildcard Scheduler/*.c) $(wildcard pipe/*.c) $(wildcard interruptions/*.c) $(wildcard Semaphores/*.c)
DIRS = -I./include -I./MemoryManager -I./processManagement -I./pipe/include -I./pipe  -I./interruptions -I./Semaphores -I./Scheduler
SOURCES_ASM= $(wildcard asm/*.asm)
OBJECTS= $(SOURCES:.c=.o)
OBJECTS_ASM= $(SOURCES_ASM:.asm=.o)
LOADERSRC = asm/loader.asm

LOADEROBJECT= $(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)


$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
%.o: %.c
	$(GCC) $(GCCFLAGS) -fno-pic $(DIRS) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	find . -name '*.o' -delete
	rm -f *.bin kernel.elf

.PHONY: all clean
