# include Makefile.inc

# KERNEL=kernel.bin
# SOURCES= $(wildcard *.c) $(wildcard MemoryManager/*.c) 
# DIRS = -I./include -I./MemoryManager
# SOURCES_ASM= $(wildcard asm/*.asm)
# OBJECTS= $(SOURCES:.c=.o)
# OBJECTS_ASM= $(SOURCES_ASM:.asm=.o)
# LOADERSRC = loader.asm

# LOADEROBJECT= $(LOADERSRC:.asm=.o)
# STATICLIBS=

# all: $(KERNEL)

# $(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
# 	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
# 	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)
# %.o: %.c
# 	$(GCC) $(GCCFLAGS) -fno-pic $(DIRS) -c $< -o $@

# %.o : %.asm
# 	$(ASM) $(ASMFLAGS) $< -o $@

# $(LOADEROBJECT):
# 	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

# clean:
# 	rm -rf asm/*.o *.o *.bin MemoryManager/*.o 

# .PHONY: all clean
include Makefile.inc

KERNEL=kernel.bin
SOURCES= $(wildcard *.c) $(wildcard MemoryManager/*.c)
DIRS = -I./include -I./MemoryManager
SOURCES_ASM= $(wildcard asm/*.asm)
OBJECTS= $(SOURCES:.c=.o)
OBJECTS_ASM= $(SOURCES_ASM:.asm=.o)
LOADERSRC = loader.asm

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
