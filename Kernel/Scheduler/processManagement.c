#include <processManagement.h>
//#include <memory_manager.h>
//#include <naiveGraphicsConsole.h>
//#include <interrupts.h>
//#include <scheduler.h>

#define DEFAULT_PRIORITY MEDIUM
#define DEFAULT_BACKGROUND BACKGROUND

static PID pid = 0;

#pragma pack(push, 1)

typedef struct {

    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

} RegState;

typedef struct {

    RegState regs;
    void* rip;
    uint64_t cs;
    uint64_t rflags;
    void* rsp;
    void* ss;
    uint64_t align;

} ProcState;

#pragma pack(pop)

static uint64_t strcpy(char* to, const char * from){
    int i;
    for(i = 0; from[i] != 0; i++)
    {
        to[i] = from[i];
    }
    return i;
}

static uint64_t strlen(const char * s){
    int i;
    for(i = 0; s[i] != 0; i++);
    return i;
}

static char* staticArgs[1024];

static PCB tempPCB;

PID processCreate(void * program, unsigned int argc, char** argv){

    void* memStart = alloc(PROC_MEM);
    
    if(memStart == NULL){
        
        return -1;
    }

    void* memEnd = (char*)memStart + PROC_MEM -1;

    for(uint8_t* i = (uint8_t*)memStart; i <= (uint8_t*)memEnd; i++)
        *i = 0;

    int argvStringsSize = 0;

    for(int i = 0; i < argc; i++){

        int size = strlen(argv[i]) + 1;
        argvStringsSize += size;
        char* dir = (char*)memEnd - argvStringsSize;
        strcpy(dir, argv[i]);
        staticArgs[i] = dir;
    }

    char** newProcArgv = (char**)((char*)memEnd - argvStringsSize) - argc;

    for(int i = 0; i < argc; i++){
        newProcArgv[i] = staticArgs[i];
    }

    void* rsp = (uint64_t*)newProcArgv - 1;
    rsp = (void*)((uint64_t)rsp % 8 == 0 ? (uint64_t)rsp : (uint64_t)rsp - (uint64_t)rsp % 8);

    ProcState* p = (ProcState*)((char*)rsp - sizeof(ProcState) + 1);
    p->cs = 8;
    p->regs.rbp = (uint64_t)rsp;
    p->rsp = rsp;
    p->rflags = 0x202;
    p->rip = program;

    p->regs.rdi = (uint64_t)argc;
    p->regs.rsi = (uint64_t)newProcArgv;

    tempPCB.pid = pid;
    tempPCB.rsp = &(p->regs);
    tempPCB.state = READY;
    tempPCB.memStart = memStart;
    tempPCB.argc = argc;
    tempPCB.argv = newProcArgv;
    tempPCB.priority = DEFAULT_PRIORITY;
    tempPCB.background = DEFAULT_BACKGROUND;

    if(pid == 1){
        tempPCB.fd[0] = 0;
        tempPCB.fd[1] = 1;
        tempPCB.fd[2] = 2;
        for(int i = 3; i < MAX_FD; i++)
            tempPCB.fd[i] = -1;
    }
    else if(pid > 1)
        forkfd(tempPCB.fd);
    pid++;

    if(schedulerAddProcess(tempPCB)< 0){
        free(memStart);
        return -1;
    }

    return tempPCB.pid;
}