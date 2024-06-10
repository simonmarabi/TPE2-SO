#include <inout.h>
#include <syscalls.h>
#include <test_util.h>

//TO BE INCLUDED
static void endless_loop()
{
  while (1) // added yield so that it runs faster
    sys_yield();
}

static uint32_t my_create_process(void *func, char *name)
{
  char *argv[] = {name};
  return sys_createprocess(func, 1, argv);
}

static uint32_t my_kill(uint32_t pid)
{
  return sys_kill(pid);
}

static uint32_t my_block(uint32_t pid)
{
  return sys_block(pid);
}

static uint32_t my_unblock(uint32_t pid)
{
  return sys_unblock(pid);
}

#define MAX_PROCESSES 3000 //Should be around 80% of the the processes handled by the kernel

enum State
{
  ERROR,
  RUNNING,
  BLOCKED,
  KILLED
};

typedef struct P_rq
{
  uint32_t pid;
  enum State state;
} p_rq;

// se saca afuera para no ocupar stack
static p_rq p_rqs[MAX_PROCESSES];

void test_processes()
{
  // se modifico porque uint8_t no alcanzaba para la cantidad de procesos
  uint64_t rq;
  uint64_t alive = 0;
  uint64_t action;

  printf("Creating %d processes..\n",MAX_PROCESSES);
  while (1)
  {

    // Create MAX_PROCESSES processes
    for (rq = 0; rq < MAX_PROCESSES; rq++)
    {
      p_rqs[rq].pid = my_create_process(&endless_loop, "endless_loop"); // TODO: Port this call as required

      if (p_rqs[rq].pid == -1)
      {                                     // TODO: Port this as required
        printf("Error creating process\n"); // TODO: Port this as required
        return;
      }
      else
      {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0)
    {

      for (rq = 0; rq < MAX_PROCESSES; rq++)
      {
        action = GetUniform(2) % 2;

        switch (action)
        {
        case 0:
          if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
          {
            if (my_kill(p_rqs[rq].pid) == -1)
            {                                    // TODO: Port this as required
              printf("Error killing process\n"); // TODO: Port this as required
              return;
            }
            p_rqs[rq].state = KILLED;
            alive--;
          }
          break;

        case 1:
          if (p_rqs[rq].state == RUNNING)
          {
            if (my_block(p_rqs[rq].pid) == -1)
            {                                     // TODO: Port this as required
              printf("Error blocking process\n"); // TODO: Port this as required
              return;
            }
            p_rqs[rq].state = BLOCKED;
          }
          break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < MAX_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2)
        {
          if (my_unblock(p_rqs[rq].pid) == -1)
          {                                       // TODO: Port this as required
            printf("Error unblocking process\n"); // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  printf("OK\n");
  }
}
