#pragma once

#include <stdint.h>
#include <memory_manager.h>

typedef uint8_t spinlock;

void initLock(spinlock * lock);
void acquire(spinlock * lock);
void release(spinlock * lock);