#include <pipe.h>

#define PIPE_BUFFER 4096
#define MAX_FIFO 64


static int semcounter = 0;

struct Pipe {
    
    char buf[PIPE_BUFFER];
    uint32_t readIdx;
    uint32_t writeIdx;
    
    int readOpen;
    int writeOpen;
    int readPID;
    int writePID;

    semaphoreID writeQueue;

}Pipe;