// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdlib.h>
#include <stdint.h>
#include <fd.h>
#include <pipe.h>
#include <memory_manager.h>
#include <scheduler.h>
#include <naiveGraphicsConsole.h>
#include <semaphores.h>

static int semcounter = 0;

struct Pipe {
    
    char buf[PIPE_BUFFER];
    uint32_t readIdx;
    uint32_t writeIdx;
    
    int readOpen;
    int writeOpen;
    int readPID;
    int writePID;

    semID writequeue;
    semID readqueue;

    int readfd;
    int writefd;

}Pipe;

typedef struct fifo{
    uint64_t id;
    uint16_t readfd;
    uint16_t writefd;
}FIFO;

static FIFO fifoTable[MAX_FIFO];

static pPipe pipeTable[MAX_FD];

int64_t mkFifo(uint64_t id){

    if(id == 0){
        return -1;
    }

    int64_t k = -1;

    for(int64_t i = 0; i < MAX_FIFO; i++){

        if(fifoTable[i].id == id){
            return -1;
        }

        if(fifoTable[i].id == 0 && k == -1){
            k = i;
        }
    }

    if(k >= 0){
        uint64_t fd[2];

        if(openPipe(fd) < 0){
            return -1;
        }

        fifoTable[k].id = id;
        fifoTable[k].readfd = fd[0];
        fifoTable[k].writefd = fd[1];
        return 0;
    }

    return -1;
}

int64_t openFifo(uint64_t id, fdType type){
    
    if(id == 0){
        return -1;
    }

    for(int64_t i = 0; i < MAX_FIFO; i++){
        
        if(fifoTable[i].id == id){
            
            if(type == READ){
                fdDup(fifoTable[i].readfd);
                return fifoTable[i].readfd;
            }
            
            if(type == WRITE){
                fdDup(fifoTable[i].writefd);
                return fifoTable[i].writefd;
            }
        }
    }

    return -1;

}

int64_t rmFifo(uint64_t id){
    
    if(id == 0){
        return -1;
    }

    for(int64_t i = 0; i < MAX_FIFO; i++){

        if(fifoTable[i].id == id){
            fifoTable[i].id = 0;
            closeFd(fifoTable[i].readfd);
            closeFd(fifoTable[i].writefd);
            fifoTable[i].readfd = 0;
            fifoTable[i].writefd = 0;
            return 0;
        }
    }

    return -1;

}


int64_t openPipe(uint64_t fd[2]){
    pPipe pipe = alloc(sizeof(struct Pipe));
    if (pipe == NULL)
        return -1;

    int64_t id;

    id = initFd(READ,pipe);
    if(id < 0) {
        free(pipe);
        return -1;
    }
    fd[0] = id;

    id = initFd(WRITE, pipe);
    if(id<0){
        free(pipe);
        return -1;
    }
    fd[1] = id;

    pipe->writeOpen = 1;
    pipe->readOpen = 1;
    pipe->readIdx = 0;
    pipe->writeIdx = 0;

    pipe->writequeue = semcounter++;
    pipe->readqueue = semcounter++;

    semOpen(pipe->writequeue, 1);
    semOpen(pipe->readqueue, 1);

    pipe->readfd = fd[0];
    pipe->writefd = fd[1];
    for(size_t i=0; i < MAX_FD; i++) {
        if (pipeTable[i] == NULL){
            pipeTable[i] = pipe;
            break;
        }
    }

    return 0;
}

void closePipe(pPipe pipe, fdType type){
    if(type == READ)
        pipe->readOpen = 0;
    else if(type == WRITE){
        pipe->writeOpen = 0;
        pipe->buf[pipe->writeIdx++ % PIPE_BUFFER] = -1;
    }

    if(!pipe->readOpen && !pipe->writeOpen){
        for(size_t i = 0;i < MAX_FD; i++){
            if(pipeTable[i] == pipe){
                pipeTable[i] = NULL;
                break;
            }
        }
        deleteSemaphore(pipe->readqueue);
        deleteSemaphore(pipe->writequeue);
        free(pipe);
    }
}

int64_t readPipe(pPipe pipe, char* buf, uint64_t count){
    if(!pipe->readOpen)
        return -1;
    
    semWait(pipe->readqueue);

    pipe->readPID = getPID();
    while (pipe->readIdx == pipe->writeIdx){
        if(!pipe->readPID){
            pipe->readPID = 0;
            semPost(pipe->readqueue);
            return 0;
        }
        blockProcess(pipe->readPID);
    }
    pipe->readPID = 0;

    int i;
    for(i = 0; i < count; i++){
        if(pipe->readIdx == pipe->writeIdx)
            break;
        if(pipe->buf[pipe->readIdx % PIPE_BUFFER] == -1)
            break;
        buf[i] = pipe->buf[pipe->readIdx++ % PIPE_BUFFER];
    }

    if(pipe->writePID){
        unblockProcess(pipe->writePID);
    }

    semPost(pipe -> writePID);
    return i;
}

int64_t writePipe(pPipe pipe, const char* buf, uint64_t count) {
    if (!pipe->writeOpen || !pipe->readOpen)
        return -1;

    semWait(pipe->writequeue);

    for(int i = 0; i < count; i++) {
        pipe->writePID = getPID();
        while(pipe->writeIdx == pipe->readIdx + PIPE_BUFFER) {
            if(!pipe->readOpen) {
                pipe->writePID = 0;
                semPost(pipe->writequeue);
                return -1;
            }
            if (pipe->readPID) 
                unblockProcess(pipe->readPID);
            blockProcess(pipe->writePID);
        }
        pipe->writePID = 0;
        pipe->buf[pipe->writeIdx++ % PIPE_BUFFER] = buf[i];
    }

    if (pipe->readPID) 
        unblockProcess(pipe->readPID);

    semPost(pipe->writequeue);
    return count;
}

void listPipes() {

    ngc_print("Lista de pipes del sistema:");
    ngc_printNewline();
    ngc_print("readfd | writefd | bytesToRead | readPid | writePid | writeQueue | readQueue");
    ngc_printNewline();
    for (size_t i = 0; i < MAX_FD; i++) {
        if (pipeTable[i] != NULL) {
            // readfd
            if (pipeTable[i]->readOpen)
                ngc_printInt(pipeTable[i]->readfd);
            else
                ngc_print("-");
            ngc_print(" | ");

            // writefd
            if (pipeTable[i]->writeOpen)
                ngc_printInt(pipeTable[i]->writefd);
            else
                ngc_print("-");
            ngc_print(" | ");

            // bytesToRead
            ngc_printInt(pipeTable[i]->writeIdx - pipeTable[i]->readIdx);
            ngc_print(" | ");

            // readPid
            if (pipeTable[i]->readPID)
                ngc_printInt(pipeTable[i]->readPID);
            else
                ngc_print("-");
            ngc_print(" | ");

            // writePid
            if (pipeTable[i]->writePID)
                ngc_printInt(pipeTable[i]->writePID);
            else
                ngc_print("-");
            ngc_print(" | ");

            // print readqueue pids
            semPrintPIDs(pipeTable[i]->readqueue);
            ngc_print(" | ");
            // print writequeue pids
            semPrintPIDs(pipeTable[i]->writequeue);

            ngc_printNewline();
        }
    }
}










