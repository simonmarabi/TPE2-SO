#pragma once
//#include <processManagent.h>
#include <stdint.h>
#include <memory_manager.h>


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

