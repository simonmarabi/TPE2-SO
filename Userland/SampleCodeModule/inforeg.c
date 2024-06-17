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
    printf("rip: %lu\n", regs->rip);
    printf("rflags: %lu\n", regs->rflags);
    printf("rbp: %lu\n", regs->rbp);
    printf("rsp: %lu\n", regs->rsp);
    printf("rax: %lu\n", regs->rax);
    printf("rbx: %lu\n", regs->rbx);
    printf("rcx: %lu\n", regs->rcx);
    printf("rdx: %lu\n", regs->rdx);
    printf("rdi: %lu\n", regs->rdi);
    printf("rsi: %lu\n", regs->rsi);
    printf("r8: %lu\n",  regs->r8);
    printf("r9: %lu\n",  regs->r9);
    printf("r10: %lu\n", regs->r10);
    printf("r11: %lu\n", regs->r11);
    printf("r12: %lu\n", regs->r12);
    printf("r13: %lu\n", regs->r13);
    printf("r14: %lu\n", regs->r14);
    printf("r15: %lu\n", regs->r15);
}
