#pragma once

#include <stdint.h>
#include <spinlock.h>
#include <processManagement.h>
#include <scheduler.h>
#include <memory_manager.h>
#include <naiveGraphicsConsole.h>


typedef int semID;

typedef struct processNode{

    PID pid;
    struct processNode * next;

} processNode;

typedef struct processQueue{

    processNode * first;
    processNode * last;

}processQueue;

typedef struct semaphore{
    
    semID id;
    uint64_t value;
    spinlock lock;
    processQueue activeQueue;
    processQueue blockedQueue;
    struct semaphore * next;

}semaphore;


int semOpen(semID id,uint64_t value);
int semWait(semID id);
int semPost(semID id);
int semClose(semID id);
void semPrintAll();
void semPrintPIDs(semID id);
int deleteSemaphore(semID id);