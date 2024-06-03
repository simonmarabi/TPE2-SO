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

    semaphoreID writequeue;
    semaphoreID readqueue;

    int readfd;
    int writefd;

}Pipe;



