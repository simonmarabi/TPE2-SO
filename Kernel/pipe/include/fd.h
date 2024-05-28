#pragma once

//#include <stdint.h>
#include <pipe.h>

#define MAX_FD 255

int64_t initFd(fdType type, pPipe pipe);
int64_t closeFd(uint64_t id);

void fdDup(uint64_t id);

int64_t readFd(uint64_t fd, char * buf, uint64_t count);
int64_t writeFd(uint64_t fd, const char * buf, uint64_t count);