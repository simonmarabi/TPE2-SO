#include <pipe.h>
#include <fd.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct fd{

    uint64_t ref;
    fdType type;
    pPipe pipe;

}FD;

static FD fdTable[MAX_FD];

int64_t initFd(fdType type, pPipe pipe){
    
    for(size_t i = 3; i < MAX_FD; i++){
        if(fdTable[i].ref == 0){
            fdTable[i].ref = 1;
            fdTable[i].type = type;
            fdTable[i].pipe = pipe;
            return i;
        }
    }

    return -1;
}

int64_t closefd(uint64_t id){
    
    if(id > MAX_FD || fdTable[id].ref == 0){
        return -1;
    }

    fdTable[id].ref--;
    
    if(fdTable[id].ref == 0){
        closePipe(fdTable[id].pipe, fdTable[id].type);
        fdTable[id].pipe = NULL;
    }

    return 0;
}

void fdDup(uint64_t id){

    if(id >= MAX_FD || fdTable[id].ref == 0){
        return;
    }

    fdTable[id].ref++;

}

int64_t readFd(uint64_t fd, char * buf, uint64_t count){
    
    if(fd >= MAX_FD || fdTable[fd].type != READ){
        return -1;
    }

    return readPipe(fdTable[fd].pipe, buf, count);
}

int64_t writeFd(uint64_t fd, const char * buf, uint64_t count){

    if(fd >= MAX_FD || fdTable[fd].type != WRITE){
        return -1;
    }

    return writePipe(fdTable[fd].pipe, buf, count);
}