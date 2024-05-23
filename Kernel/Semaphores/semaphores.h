#pragma once

#include <stdint.h>
#include <spinlock.h>
#include <processManagement.h>
#include <scheduler.h>
#include <memory_manager.h>


typedef int semaphoreID;

typedef struct processNode
{
    PID pid;
    struct processNode * next;
} processNode;
typedef struct processQueue
{
    processNode * first;
    processNode * last;
}processQueue;

typedef struct semaphore
{
    semaphoreID id;
    uint64_t value;
    spinlock lock;
    processQueue activeQueue;
    processQueue blockedQueue;
    struct semaphore * next;

}semaphore;

static semaphore * semaphoreList = NULL;

int semaphoreOpen(semaphoreID id,uint64_t value);
int semaphoreWait(semaphoreID id);
int semaphorePost(semaphoreID id);
int semaphoreClose(semaphoreID id);
void semaphorePrintAll();
int semaphorePrintPIDs(semaphoreID id);
void deleteSemaphore(semaphoreID id);