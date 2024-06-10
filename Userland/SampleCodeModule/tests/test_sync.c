#include <inout.h>
#include <stringUtil.h>
#include <syscalls.h>

#define N 100000

static uint32_t my_create_process(void* func, int argc, char** argv){
  return sys_createprocess(func, argc, argv);
}

static uint64_t my_sem_open(int sem_id, uint64_t initialValue){
  return sys_semopen(sem_id, initialValue);
}

static uint64_t my_sem_wait(int sem_id){
  return sys_semwait(sem_id);
}

static uint64_t my_sem_post(int sem_id){
  return sys_sempost(sem_id);
}

static uint64_t my_sem_close(int sem_id){
  return sys_semclose(sem_id);
}

#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID 30

static int64_t global;  //shared memory

static void slowInc(int64_t *p, int64_t inc){
  int64_t aux = *p;
  aux += inc;
  sys_yield();
  *p = aux;
}

static void inc(int argc, char** argv){
  uint64_t sem; int64_t value;
  strToIntBase(argv[1], _strlen(argv[1]), 10, (int*)&sem, 1);
  strToIntBase(argv[2], _strlen(argv[2]), 10, (int*)&value, 0);

  uint64_t i;

  if (sem && my_sem_open(SEM_ID, 1) < 0){
    printf("ERROR OPENING SEM\n");
    return;
  }
  
  for (i = 0; i < N; i++){
    if (sem) my_sem_wait(SEM_ID);
    slowInc(&global, value);
    if (sem) my_sem_post(SEM_ID);
  }

  if (sem) my_sem_close(SEM_ID);
  
  printf("Final value: %d\n", global);
  sys_exit();
}

void test_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...(WITH SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    char* argv1[] = {"inc", "1", "1"};
    char* argv2[] = {"inc", "1", "-1"};

    my_create_process(&inc, 3, argv1);
    my_create_process(&inc, 3, argv2);
  }
}

void test_no_sync(){
  uint64_t i;

  global = 0;

  printf("CREATING PROCESSES...(WITHOUT SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    char* argv1[] = {"inc", "0", "1"};
    char* argv2[] = {"inc", "0", "-1"};

    my_create_process(&inc, 3, argv1);
    my_create_process(&inc, 3, argv2);
  }
}
