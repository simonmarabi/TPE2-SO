#ifndef SYSCALLS_H
#define SYSCALLS_H
#include <stdint.h>
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

typedef int PID;

typedef int semID;

typedef enum Priority
{
    LOW = 1,
    MEDIUM,
    HIGH,
} Priority;


typedef enum Background
{
    FOREGROUND=0,
    BACKGROUND=1
}Background;

typedef enum fdType
{
    READ =0,
    WRITE
}fdType;


extern void sys_write(uint64_t fd, const char * buffer, uint64_t count);

extern int64_t sys_read(uint64_t fd, const char * buffer, uint64_t count);

uint64_t sys_time();

void sys_setTerminalRawMode(uint8_t setRaw);

void sys_changeFontSize(uint64_t level);

void sys_printColor(const char * buffer, uint64_t color);

void sys_getScreenData(uint16_t * screen);

void sys_paint_rect(uint16_t fromX, uint16_t fromY, uint16_t width, uint16_t height, uint64_t color);

uint64_t sys_getTicks();
 
void sys_clear_screen(uint64_t color);

void sys_beeper(uint64_t frequency, uint64_t seconds);

void sys_printMem();

void * sys_alloc(unsigned long size);

void sys_free(void * memptr);

int sys_semopen(semID id, uint64_t value);

int64_t sys_close(uint64_t fd);

PID sys_getpid();

int sys_mapstdfds(PID pid, int stdin, int stdout);

int sys_semclose(semID id);

void sys_exit();

PID sys_createprocess(void* program, unsigned int argc, char** argv);

int64_t sys_chgpriority(PID pid, Priority priority);

int sys_sempost(semID id);

int sys_semwait(semID id);

int64_t sys_pipe(uint64_t fd[2]);

int64_t sys_kill(PID pid);

int sys_setbackground(PID pid, Background background);

int64_t sys_block(PID pid);

int64_t sys_unblock(PID pid);

void sys_listprocesses();

void sys_listpipes();

void sys_listsem();

void sys_yield();

#endif