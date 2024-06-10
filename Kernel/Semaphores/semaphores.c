#include <semaphores.h>

static semaphore * semaphoreList = NULL;

static semaphore * searchSemaphore(semaphoreID searchedID)
{
    semaphore * iterator = semaphoreList;
    
    while (iterator != NULL)
    {
        if (iterator->id == searchedID)
        {
            return iterator;
        }
        iterator = iterator->next;
    }
    return iterator;
}

static void removeSemaphore (semaphoreID idToRemove)
{
    semaphore * iterator = semaphoreList;
    if (iterator == NULL) return;
    //Eliminamos el primero
    if (iterator->id == idToRemove)
    {
        semaphoreList = iterator->next;
        free(iterator);
        return;
    }
    while (iterator->next != NULL)
    {
        if (iterator->next->id == idToRemove)
        {
            semaphore * aux = iterator->next;
            iterator->next = iterator->next->next;
            free(aux);
            return;
        }
        iterator = iterator->next;
    }
    
}

static void addToProcessQueue(processQueue * queue, PID pid, int isActiveQueue)
{
    if (isActiveQueue)
    {
        processNode* iterator = queue->first;
        while(iterator != NULL)
        {
            //No agrego dos veces al mismo
            if (iterator->pid == pid) return;

            iterator= iterator->next;
        }
    }

    processNode * toAdd = alloc(sizeof(processNode));
    if (toAdd == NULL) return;
    toAdd->pid = pid;
    toAdd->next = NULL;
    if (queue->first == NULL)
        queue->first = toAdd;
    else 
        queue->last = toAdd;
}

static void removeFromQueue(processQueue * queue,PID pid)
{
    processNode * iterator = queue->first;
    if (iterator == NULL) return;

    if (iterator->pid == pid)
    {
        queue->first = iterator->next;
        if (iterator->next == NULL)
            queue->last = NULL;
        free(iterator);
        return;
    }
    while (iterator->next != NULL)
    {
        if (iterator->next->pid == pid)
        {
            processNode * aux = iterator->next;
            iterator->next = iterator->next->next;
            if (queue->last == aux)
                queue->last = iterator;
            free(aux);
            return;
        }
        iterator = iterator->next;
    }
    
}

static PID popBlockedQueue(processQueue * queue)
{
    if (queue->first == NULL)
        return -1;
    processNode * toRemove = queue->first;
    PID pid = toRemove->pid;
    queue->first = queue->first->next;
    if (queue->first == NULL)
        queue->last = NULL;
    free(toRemove);
    return pid;
}

static void addSemaphore(semaphore * toAdd){
    
    semaphore * iterator = semaphoreList;
    if (iterator == NULL)
    {
        semaphoreList = toAdd;
        return;
    }
    while (iterator->next != NULL)
    {
        iterator = iterator->next;
    }

    iterator->next = toAdd;
}

int semaphoreOpen(semaphoreID id,uint64_t value)
{
    semaphore * toAdd = searchSemaphore(id);
    if (toAdd != NULL)
    {
        addToProcessQueue(&(toAdd->activeQueue), getPID() ,1);
        return 0;
    }
    toAdd = alloc(sizeof(semaphore));
    if (toAdd ==  NULL)
        return -1;
    toAdd->id = id;
    toAdd->value = value;
    toAdd->blockedQueue.first = NULL;
    toAdd->blockedQueue.last = NULL;
    toAdd->activeQueue.first = NULL;
    toAdd->activeQueue.last = NULL;
    toAdd->next = NULL;
    addToProcessQueue(&(toAdd->activeQueue),getPID(),1);
    initLock(&toAdd->lock);
    addSemaphore(toAdd);
    return 1;
}

static void wakeup(processQueue *queue)
{
    PID pid = popBlockedQueue(queue);
    if (pid < 0) return;
    unblockProcess(pid);
}

static void sleep(processQueue *queue)
{
    PID pid = getPID();
    addToProcessQueue(queue,pid,0);
    blockProcess(pid);

}

static int verifyPID(processQueue * activeQueue, PID pid)
{
    processNode * iterator = activeQueue->first;
    while (iterator != NULL)
    {
        if (iterator->pid == pid) return 1;
        
        iterator = iterator->next;
    }
    return 0;
}

static void printStats(semaphore *sem)
{
    ngc_printInt(sem->id);
    ngc_print(" | ");
    ngc_printInt(sem->value);
    ngc_print(" | ");

    processNode * iterator = sem->blockedQueue.first;
    if (iterator == NULL)
    {
        ngc_print("-");
        ngc_printNewline();

        return;
    }
    while(iterator != NULL)
    {
        ngc_printInt(iterator->pid);
        ngc_print(" ");
        iterator = iterator->next;
    }
    ngc_printNewline();
}

void semaphorePrintPIDs(semaphoreID id)
{
    semaphore * iterator = semaphoreList;
    while (iterator != NULL)
    {
        if (iterator->id == id)
        {
            processNode * it = iterator->blockedQueue.first;
            if (it==NULL)
            {
                ngc_print("-");
                return;
            }
            while (it != NULL)
            {
                ngc_printInt(it->pid);
                ngc_print(" ");
                it = it->next;
            }
            return;
        }
        iterator=iterator->next;
        }
        
    }

int semaphoreWait(semaphoreID id)
{
    semaphore * sem = searchSemaphore(id);
    if (sem == NULL)
    {
        return -1;
    }
    acquire(&(sem->lock));
    if (sem->value > 0)
        sem->value -= 1;
    else
    {
        release(&(sem->lock));
        sleep(&(sem->blockedQueue));
        acquire(&(sem->lock));
        sem->value -= 1;
    }
    release(&(sem->lock));
    return 0;
    
}
int semaphorePost(semaphoreID id)
{
    semaphore * sem = searchSemaphore(id);
    if (sem == NULL) return -1;
    acquire(&(sem->lock));
    sem->value += 1;
    wakeup(&(sem->blockedQueue));
    release(&(sem->lock));
    yield();
    return 0;
}
int semaphoreClose(semaphoreID id)
{
    semaphore * sem = searchSemaphore(id);
    if (sem == NULL) return -1;

    if (!verifyPID(&(sem->activeQueue),getPID())) return -2;

    removeFromQueue(&(sem->activeQueue),getPID());
    if (sem->activeQueue.first == NULL) removeSemaphore(sem->id);

    return 1;
}

void semaphorePrintAll()
{
    ngc_print("SEMAFORE ID | VALUE | BLOCKED PROCESSES\n");
    semaphore * it = semaphoreList;
    
    while (it != NULL)
    {
        printStats(it);
        it = it->next;
    }
    
}

int deleteSemaphore(semaphoreID id)
{
    semaphore * sem = searchSemaphore(id);
    if(sem == NULL)
        return -1;
    
    removeSemaphore(sem->id);
    return 1;
}