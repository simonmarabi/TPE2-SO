#pragma once

#include <stddef.h>
#include <stdint.h>

void * alloc(unsigned long size);

void free(void * memptr);

#ifdef BUDDY

void setupBuddy();

#endif

void printMem();
