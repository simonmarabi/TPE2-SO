#include <inout.h>
#include <syscalls.h>

#define MINOR_WAIT 5000000                               // TODO: To prevent a process from flooding the screen
#define WAIT      100000000                              // TODO: Long enough to see theese processes beeing run at least twice

static uint64_t my_getpid(){
  return sys_getpid();
}

static uint64_t my_create_process(void* func, char * name){
  char* argv[] = {name};
  return sys_createprocess(func, 1, argv);
}

static uint64_t my_nice(uint64_t pid, uint64_t newPrio){
  return sys_chgpriority(pid, newPrio);
}

static uint64_t my_kill(uint64_t pid){
  return sys_kill(pid);
}

static uint64_t my_block(uint64_t pid){
  return sys_block(pid);
}

static uint64_t my_unblock(uint64_t pid){
  return sys_unblock(pid);
}

static void bussy_wait(uint64_t n){
  uint64_t i;
  for (i = 0; i < n; i++);
}

static void endless_loop(){
  uint64_t pid = my_getpid();

  while(1){
    printf("%d ",pid);
    bussy_wait(MINOR_WAIT);
  }
}

#define TOTAL_PROCESSES 3

void test_prio(){
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  for(i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = my_create_process(&endless_loop, "endless_loop");

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 1); //lowest priority 
        break;
      case 1:
        my_nice(pids[i], 2); //medium priority
        break;
      case 2:
        my_nice(pids[i], 3); //highest priority
        break;
    }
  }

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++) {
    printf("BLOCKING PID %d\n",pids[i]);
    my_block(pids[i]);
  }

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");
  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        my_nice(pids[i], 2); //medium priority 
        break;
      case 1:
        my_nice(pids[i], 2); //medium priority
        break;
      case 2:
        my_nice(pids[i], 2); //medium priority
        break;
    }
  }

  printf("UNBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    my_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++) {
    printf("KILLING PID %d\n",pids[i]);
    my_kill(pids[i]);
  }
}
