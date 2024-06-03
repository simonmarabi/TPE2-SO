#pragma once

#include <stdint.h>

#define PIPE_BUFFER 4096
#define MAX_FIFO 64

typedef enum fdType {

    READ = 0,
    WRITE,
    
}fdType;

typedef struct Pipe * pPipe;

//PIPE FUNCTION
int64_t openPipe (uint64_t fd[2]);
void closePipe(pPipe pipe, fdType type);
int64_t readPipe(pPipe pipe, char* buffer, uint64_t counter);
int64_t writePipe(pPipe pipe,const char* buffer, uint64_t counter);


int64_t openFifo(uint64_t id,fdType type);
int64_t mkFifo(uint64_t id);

int64_t rmFifo(uint64_t id);

void listPipes();
