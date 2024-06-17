// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    printf("rip: %lx\n", (uint64_t)regs->rip);
    printf("rflags: %lx\n", (uint64_t)regs->rflags);
    printf("rbp: %lx\n", (uint64_t)regs->rbp);
    printf("rsp: %lx\n", (uint64_t)regs->rsp);
    printf("rax: %lx\n", (uint64_t)regs->rax);
    printf("rbx: %lx\n", (uint64_t)regs->rbx);
    printf("rcx: %lx\n", (uint64_t)regs->rcx);
    printf("rdx: %lx\n", (uint64_t)regs->rdx);
    printf("rdi: %lx\n", (uint64_t)regs->rdi);
    printf("rsi: %lx\n", (uint64_t)regs->rsi);
    printf("r8: %lx\n", (uint64_t)regs->r8);
    printf("r9: %lx\n", (uint64_t)regs->r9);
    printf("r10: %lx\n", (uint64_t)regs->r10);
    printf("r11: %lx\n", (uint64_t)regs->r11);
    printf("r12: %lx\n", (uint64_t)regs->r12);
    printf("r13: %lx\n", (uint64_t)regs->r13);
    printf("r14: %lx\n", (uint64_t)regs->r14);
    printf("r15: %lx\n", (uint64_t)regs->r15);
}
