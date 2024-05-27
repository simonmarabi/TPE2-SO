#pragma once

#include "processManagement.h"

typedef struct ProcessNode {
    
    struct ProcessNode * prev;
    struct ProcessNode * next;

    PCB pcb;
    size_t priorityCounter;
    
}ProcessNode;

typedef struct{
   ProcessNode * first; 
} BlockedList;

typedef struct {
    ProcessNode * current;
    size_t counter;
}ReadyList;

static ReadyList readyList;
static BlockedList blockedList;

static void * haltRsp;

void * scheduler(void * rsp);
int schedulerAddProcess(PCB pcb);
PID getPID(void);
Background getBackground();
int setBackground(PID pid, Background Background);
int64_t blockProcess(PID pid);
int64_t unblockProcess(PID pid);
int64_t terminateProcess(PID pid);
int64_t changePriority(PID pid,Priority priority);
void yeild(void);
void _int20(void);
int mapStdFds(PID pid, int stdin, int stdout);
int fdLocalToGlobal(int fd);
void addFd(uint32_t fd);
void listProcesses();
void forkfd(int fd[]);