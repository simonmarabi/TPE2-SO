#include <stdint.h>
#include <scheduler.h>
#include <stdlib.h>
#include <memory_manager.h>
#include <interrupts.h>
#include <naiveGraphicsConsole.h>


static ReadyList readyList;
static BlockedList blockedList;

static void * haltRsp;
static uint64_t halt = 0;

void forkfd(int fd[]){
    for(int i = 0; i < MAX_FD; i++){
        fd[i] = readyList.current -> pcb.fd[i];
        fdDup(fd[i]);
    }
}

int schedulerAddProcess(PCB pcb)
{

    if(pcb.pid == 0){
        haltRsp = pcb.rsp;
        return 0;
    }
    
    ProcessNode * newReady = alloc(sizeof(ProcessNode));
    if(newReady == NULL){
        ngc_print("NO MORE MEMORY TO CREATE PROCESS");
        return -1;
    }


    newReady -> pcb = pcb;
    newReady -> priorityCounter = pcb.priority;


    if(readyList.counter == 0){
        newReady -> next = newReady;
        newReady -> prev = newReady;
        readyList.current = newReady;
    }
    else{
        newReady -> next = readyList.current;
        newReady -> prev = readyList.current -> prev;

        readyList.current -> prev -> next = newReady;
        readyList.current -> prev = newReady;
    }

    readyList.counter++;
    
    return 0;
}

PID getPID(){
    return readyList.current -> pcb.pid;
}

ProcessNode * searchReadyNode(PID pid){
    ProcessNode * iterator = readyList.current;
    for(int i = 0; i < readyList.counter; i++, iterator = iterator -> next){
        if(iterator -> pcb.pid == pid){
            return iterator;
        }
    }
    return NULL;
}

ProcessNode * searchBlockNode(PID pid){
    ProcessNode * iterator = blockedList.first;
    while(iterator != NULL){
        if(iterator -> pcb.pid == pid){
            return iterator;
        }
        iterator = iterator -> next;
    }
    return NULL;
}

ProcessNode * searchNode(PID pid){
    ProcessNode * node = searchReadyNode(pid);
    if(node == NULL){
        node = searchBlockNode(pid);
    }
    if(node == NULL){
        return NULL;
    }
    return node;
}


int setBackground(PID pid, Background Background){

    ProcessNode * node = searchNode(pid);
    
    if(node == NULL){
        return -1;
    }

    node -> pcb.background = Background;
    return 0;
}

Background getBackground(){
    return readyList.current -> pcb.background; 
}

int mapStdFds(PID pid, int stdin, int stdout){
    ProcessNode * node = searchNode(pid);
    if(node == NULL){
        return -1;
    }
    if(node -> pcb.fd[stdin] == -1 || node -> pcb.fd[stdout] == -1){
        return -1;
    }
    node -> pcb.fd[0] = stdin;
    node -> pcb.fd[1] = stdout;
    
    return 0;
}

int fdLocalToGlobal(int fd){
    return readyList.current -> pcb.fd[fd];
}

void addFd(uint32_t fd){
    if(fd >= MAX_FD){
        return;
    }
    readyList.current -> pcb.fd[fd] = fd;
}

void removeFd(uint32_t fd){
    if(fd >= MAX_FD){
        return;
    }
    readyList.current -> pcb.fd[fd] = -1;    
}

int64_t changePriority(PID pid, Priority priority){

    ProcessNode * process = searchReadyNode(pid);
    if(!process){
        process = searchBlockNode(pid);
    }
    if(process){
        process -> pcb.priority = priority;
        if(process -> priorityCounter > priority){
            process -> priorityCounter = priority;
        }
        return priority;
    }

    return -1;
}

void removeBlocked(ProcessNode * blocked){
    if(blocked -> pcb.pid == readyList.current -> pcb.pid){
        if(readyList.counter > 1){
            blocked -> prev -> next = blocked -> next;
            blocked -> next -> prev = blocked -> prev;
            readyList.current = readyList.current -> next;
        }
        else{
            readyList.current = NULL;
        }
    }
    else{
        blocked -> prev -> next = blocked -> next;
        blocked -> next -> prev = blocked -> prev;
    }

    readyList.counter--;

    blocked -> next = NULL;
    blocked -> prev = NULL;
    blocked -> priorityCounter = blocked -> pcb.priority;

    if(blockedList.first == NULL){
        blockedList.first = blocked;
    }
    else{
        blocked -> next = blockedList.first;
        blockedList.first -> prev = blocked;
        blockedList.first = blocked;
    }
}

int64_t blockProcess(PID pid){
    if(readyList.current -> pcb.pid == pid){
        readyList.current -> pcb.state = BLOCKED;
        _int20();
        return 0;
    }
    ProcessNode * process = searchReadyNode(pid);
    if (process) {
        process->pcb.state = BLOCKED;
        removeBlocked(process);
        return 0;
    }
    return -1;
}

int64_t unblockProcess(PID pid){
    
    ProcessNode * process = searchBlockNode(pid);
    
    if(process){
        process -> pcb.state = READY;

        if(process -> pcb.pid == blockedList.first -> pcb.pid){
            blockedList.first = blockedList.first -> next;
            if(blockedList.first != NULL){
                blockedList.first -> prev = NULL;
            }
        }
        else{
            process -> prev -> next = process -> next;
            if(process -> next != NULL){
                process -> next -> prev = process -> prev;
            }
        }

        if(readyList.counter == 0){
            process -> next = process;
            process -> prev = process;
            readyList.current = process;
        }
        else{
            process -> prev = readyList.current;
            process -> next = readyList.current -> next;
            readyList.current -> next -> prev = process;
            readyList.current -> next = process;
        }

        readyList.counter++;

        return 0;
    }


    return -1;
}

void removeTerminated(ProcessNode * terminated){

    if (terminated->pcb.pid == readyList.current->pcb.pid)
    {
        if(readyList.counter > 1)
        {
            readyList.current->prev->next = readyList.current->next;
            readyList.current->next->prev = readyList.current->prev;
            readyList.current = readyList.current->next;

        }
        else 
        {
            readyList.current = NULL;
        }
        readyList.counter--;
    }
    else if (terminated->pcb.state == BLOCKED)
    {
        if (terminated->pcb.pid == blockedList.first->pcb.pid)
        {
            blockedList.first = blockedList.first->next;
            if (blockedList.first == NULL)
            {
                blockedList.first->prev = NULL;
            }
        }
        else
        {
            terminated->prev->next = terminated->next;
            if (terminated->next != NULL)
            {
                terminated->next->prev = terminated->prev;
            }
        }

    }
    else
    {
        terminated->prev->next = terminated->next;
        terminated->next->prev = terminated->prev;
        readyList.counter--;
    }

    for (size_t i = 3; i < MAX_FD; i++)
    {
        closeFd(terminated->pcb.fd[i]);
    }
    free(terminated->pcb.memStart);
    free(terminated);
}



int64_t terminateProcess(PID pid)
{
    if(pid == 1)
        return -1;

    if(readyList.current->pcb.pid == pid)
    {
        readyList.current->pcb.state = TERMINATED;

        if (readyList.current->pcb.background == FOREGROUND)
        {
            setBackground(1,FOREGROUND);
            unblockProcess(1);
        }

        _int20();
        return 0;

    }
    ProcessNode * process = searchReadyNode(pid);
    if (process)
    {
        if (process->pcb.background == FOREGROUND)
        {
            setBackground(1,FOREGROUND);
            unblockProcess(1);            
        }

        process->pcb.state = TERMINATED;
        removeTerminated(process);
        return 0;
    }
    process = searchBlockNode(pid);
    if (process)
    {
        if (process->pcb.background == FOREGROUND)
        {
            setBackground(1,FOREGROUND);
            unblockProcess(1);
        }
        removeTerminated(process);
        return 0;
    }
    return -1;
}


static int firstTime = 1;

void * scheduler(void * rsp)
{
    if (haltRsp == 0) return rsp;

    if (halt) haltRsp = rsp;

    else if (!firstTime)
    {
        if (rsp < readyList.current->pcb.memStart)
        {
            ngc_print("Memory Collision");
            haltcpu();
        }
        readyList.current->pcb.rsp = rsp;
    }
    halt = 0;
    firstTime =0;

    if (readyList.counter == 0)
    {
        halt = 1;
        return haltRsp;
    }
    if (readyList.current->pcb.state == TERMINATED) 
    {
        removeTerminated(readyList.current);
    }
    else if (readyList.current->pcb.state == BLOCKED) 
    {
        removeBlocked(readyList.current);
    }
    else if (readyList.current->priorityCounter == 0)
    {
        readyList.current->priorityCounter = readyList.current->pcb.priority;
        readyList.current = readyList.current->next;
    }
    readyList.current->priorityCounter--;

    if (readyList.counter == 0)
    {
        halt = 1;
        return haltRsp;
    }
    return readyList.current->pcb.rsp;
}

void yield()
{
    readyList.current->priorityCounter = 0;
    _int20();
}

void printProcess(ProcessNode * process)
{
    // PIDS
    ngc_printInt(process->pcb.pid);
    ngc_print(" | ");
    
    // name
    ngc_print(process->pcb.argv[0]);
    ngc_print(" | ");

    switch (process->pcb.state)
    {
        case READY:
            ngc_print("READY");
            break;
            
        case TERMINATED:
            ngc_print("TERMINATED");
            break;
        case BLOCKED:
            ngc_print("BLOCKED");
        break;
    }
    ngc_print(" | ");
    //rsp
    ngc_print("0x");
    ngc_printHex((uint64_t)(process->pcb.rsp));
    //Start Stack
    ngc_print("0x");
    ngc_printHex((uint64_t)(process->pcb.memStart) + PROC_MEM);
    
    ngc_print(" | ");

    //Priority
    switch(process->pcb.priority)
    {
        case LOW:
            ngc_print("LOW");
        break;
        case MEDIUM:
            ngc_print("MEDIUM");
        break;
        case HIGH:
            ngc_print("HIGH");
        break;
    }

    ngc_print(" | ");
    
    //fg/bg
    switch(process->pcb.background)
    {
        case BACKGROUND:
            ngc_print("Background");
            break;
        case FOREGROUND:
            ngc_print("Foreground");
            break;
    }
    ngc_printNewline();

}

void listProcesses()
{
    ngc_print("Lista de procesos del sistema");
    ngc_printNewline();
    ngc_print("pid | name | state | rsp | stackStart | priority | fg/bg");
    ngc_printNewline();

    ProcessNode * iterator = readyList.current;

    for (int i = 0 ; i < readyList.counter; i++, iterator = iterator->next)
    { 
        printProcess(iterator);
    }
    iterator = blockedList.first;
    while(iterator != NULL)
    {
        printProcess(iterator);
        iterator = iterator->next;
    }
}