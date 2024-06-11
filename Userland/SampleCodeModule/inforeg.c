#include <stdint.h>
#include <inout.h>

#pragma pack(push,1)
typedef struct {
    uint64_t rsp;
    uint64_t rax;
    uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
    uint64_t rflags;
    uint64_t rbp;
    uint64_t rip;
} Registers;
#pragma pack(pop)

void inforeg_print(const Registers* regs) {
    printf("rip: %x\n", regs->rip);
    printf("rflags: %x\n", regs->rflags);
    printf("rbp: %x\n", regs->rbp);
    printf("rsp: %x\n", regs->rsp);
    printf("rax: %x\n", regs->rax);
    printf("rbx: %x\n", regs->rbx);
    printf("rcx: %x\n", regs->rcx);
    printf("rdx: %x\n", regs->rdx);
    printf("rdi: %x\n", regs->rdi);
    printf("rsi: %x\n", regs->rsi);
    printf("r8: %x\n", regs->r8);
    printf("r9: %x\n", regs->r9);
    printf("r10: %x\n", regs->r10);
    printf("r11: %x\n", regs->r11);
    printf("r12: %x\n", regs->r12);
    printf("r13: %x\n", regs->r13);
    printf("r14: %x\n", regs->r14);
    printf("r15: %x\n", regs->r15);
}
