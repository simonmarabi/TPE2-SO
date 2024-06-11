#include <stdint.h>
#include <stdio.h>
#include "syscalls.h"
#include "test_util.h"

#define SEM_ID 5
#define TOTAL_PAIR_PROCESSES 3

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc)
{
  uint64_t aux = *p;
  aux += inc;
  sys_yield(); // This makes the race condition highly probable
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[])
{
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3){
    printf("This test requires 3 parameters (increments, processes, sem on/off)\n");
    return -1;
  }
  
  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
    if (sys_semopen(SEM_ID, 1) == -1)
    {
      printf("test_sync: ERROR opening semaphore\n");
      sys_exit();
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++)
  {
    if (use_sem)
      sys_semwait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      sys_sempost(SEM_ID);
  }

  if (use_sem)
    sys_semclose(SEM_ID);

  
  sys_exit();
  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[])
{ //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];
    
  if (argc < 3){
    printf("This test requires 2 parameters (increment, sem on/off)\n");
    return -1;
  }

  char *argvDec[] = {argv[1], "-1", argv[2], NULL};
  char *argvInc[] = {argv[1], "1", argv[2], NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    pids[i] = sys_createprocess(&my_process_inc, 3, argvDec);
    pids[i + TOTAL_PAIR_PROCESSES] = sys_createprocess(&my_process_inc, 3, argvInc);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
  {
    sys_semwait(pids[i]);
    sys_semwait(pids[i + TOTAL_PAIR_PROCESSES]);
  }
  printf("Final value: %d\n", global);
  return 0;
}