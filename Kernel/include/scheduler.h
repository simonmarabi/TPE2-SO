#pragma once

#include <processManagement.h>
#include <stdint.h>
#include <stdlib.h>

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


void * scheduler(void * rsp);
int schedulerAddProcess(PCB pcb);
PID getPID(void);
Background getBackground();
int setBackground(PID pid, Background Background);
int64_t blockProcess(PID pid);
int64_t unblockProcess(PID pid);
int64_t terminateProcess(PID pid);
int64_t changePriority(PID pid,Priority priority);
void yield(void);
void _int20(void);
int mapStdFds(PID pid, int stdin, int stdout);
int fdLocalToGlobal(int fd);
void addFd(uint32_t fd);
void listProcesses();
void forkfd(int fd[]);
void removeFd(uint32_t fd);