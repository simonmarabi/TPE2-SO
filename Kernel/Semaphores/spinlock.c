#include <spinlock.h>

void initLock(spinlock *lock)
{
    *lock = 0;
}