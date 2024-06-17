// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <defs.h>
#include <string.h>
#include <keyboard.h>
#include <naiveRTC.h>
#include <naiveGraphicsConsole.h>
#include <time.h>
#include <speaker.h>
#include <interrupts.h>
#include <memory_manager.h>
#include <pipe.h>
#include <semaphores.h>
#include <processManagement.h>

#define SEM_OFFSET 10000
static uint8_t rawMode = 0;

extern uint64_t info[17];
uint8_t screenshot = 1;

typedef int64_t (*syscallT)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// Función para manejar la escritura en el sistema
void sys_write_handler(uint64_t fd, uint64_t buffer, uint64_t bytes)
{
    fd = fdLocalToGlobal(fd);

    if (fd == STDOUT)
    {
        for (uint64_t i = 0; i < bytes; i++)
        {
            ngc_printChar(((char *)buffer)[i]);
        }
    }
}

// Función para manejar la lectura desde el sistema
int64_t sys_read_handler(uint64_t fd, char *buffer, uint64_t bytes)
{
    fd = fdLocalToGlobal(fd);
    PID pid = getPID();
    if (fd == STDIN)
    {
        if (getBackground())
        {
            blockProcess(pid);
            return 0;
        }

        if (rawMode)
        {
            int i;
            for (i = 0; i < bytes; i++)
            {
                buffer[i] = readAscii();
            }
            return i;
        }

        int i;
        for (i = 0; i < bytes; i++)
        {
            char c = readAscii();
            if (c == 3)
            { // CTRL + C
                terminateProcess(pid);
                return 1;
            }
            if (c == 4)
            { // CTRL + D
                return i;
            }
            if (c == '\t')
            {
                i--;
                continue;
            }
            if (c == -1)
                return -1;

            // Backspace
            if (c == 8)
            {
                if (i <= 0)
                {
                    i--;
                    continue;
                }
                i -= 2;
            }
            else
                buffer[i] = c;

            ngc_printChar(c);
            if (c == '\n')
            {
                i++;
                break;
            }
        }
        return i;
    }
    return readFd(fd, buffer, bytes);
}

// Función para obtener la hora actual del sistema
static uint64_t sys_time_handler()
{
    return (_NRTCGetHours()) | ((uint64_t)_NRTCGetMins() << 8) | ((uint64_t)_NRTCGetSeconds() << 16);
}

// Función para obtener información de los registros
static void sys_setTerminalRawMode_handler(uint8_t setRaw)
{
    rawMode = setRaw != 0;
}

// Función para cambiar el tamaño de la fuente
static void sys_font_handler(uint64_t level)
{
    changeFontSize(level);
}

// Función para imprimir texto con un color específico
static void sys_printColor_handler(const char *buffer, uint64_t color)
{
    ngc_printColor(buffer, color);
}

// Función para limpiar la pantalla con un color específico
static void sys_clear_screen_handler(uint64_t color)
{
    ngc_paint_screen(color);
}

// 2 por altura y ancho
// Función para obtener información sobre las dimensiones de la pantalla
static void sys_screenData_handler(uint16_t *data)
{
    data[0] = ngc_getWidth();
    data[1] = ngc_getHeight();
    return;
}

// Función para pintar un rectángulo en la pantalla
static void sys_paint_rect_handler(uint64_t fromX, uint64_t fromY, uint16_t width, uint16_t height, uint64_t color)
{
    ngc_print_pixels(fromX, fromY, width, height, color);
}

// Función para obtener el número de "ticks" del sistema
static uint64_t sys_ticks_handler()
{
    return ticks_elapsed();
}

// Función para activar un "beeper" con una frecuencia y duración específicas
static void sys_beeper_handler(uint64_t frequency, uint64_t interval)
{
    beep(frequency);
    wait(interval);
    stopBeep();
}

static void sys_memstate_handler()
{
    printMem();
}

static void *sys_alloc_handler(unsigned long size)
{
    return alloc(size);
}

static void sys_free_handler(void *memptr)
{
    free(memptr);
}

void sys_exit_handler()
{
    terminateProcess(getPID());
}

PID sys_getPid_handler()
{
    return getPID();
}

int64_t sys_kill_handler(PID pid)
{
    return terminateProcess(pid);
}

int64_t sys_changePriority_handler(PID pid, Priority priority)
{
    return changePriority(pid, priority);
}

int64_t sys_blockProcess_handler(PID pid)
{
    return blockProcess(pid);
}

int64_t sys_unblockProcess_handler(PID pid)
{
    return unblockProcess(pid);
}

void sys_yield_handler(void)
{
    yield();
}

int sys_setBackground_handler(PID pid, Background background)
{
    return setBackground(pid, background);
}

int64_t sys_openPipe_handler(uint64_t fd[2])
{
    int result = openPipe(fd);
    if (result >= 0)
    {
        addFd(fd[0]);
        addFd(fd[1]);
    }
    return result;
}

int64_t sys_close_handler(uint64_t fd)
{
    fd = fdLocalToGlobal(fd);
    int result = closeFd(fd);
    if (result >= 0)
        removeFd(fd);
    return result;
}

int64_t sys_mkFifo_handler(uint64_t id)
{
    return mkFifo(id);
}

int64_t sys_openFifo_handler(uint64_t id, fdType type)
{
    int64_t fd = openFifo(id, type);
    if (fd >= 0)
        addFd(fd);
    return fd;
}

int sys_mapStdFds_handler(PID pid, int stdin, int stdout)
{
    return mapStdFds(pid, stdin, stdout);
}

int64_t sys_rmFifo_handler(uint64_t id)
{
    return rmFifo(id);
}

void sys_listPipes_handler()
{
    listPipes();
}

int sys_semOpen_handler(semID id, uint64_t value)
{
    id += SEM_OFFSET;
    if (id + SEM_OFFSET < id)
        return -1;
    return semOpen(id, value);
}

int sys_semWait_handler(semID id)
{
    id += SEM_OFFSET;
    if (id + SEM_OFFSET < id)
        return -1;
    return semWait(id);
}

int sys_semPost_handler(semID id)
{
    id += SEM_OFFSET;
    if (id + SEM_OFFSET < id)
        return -1;
    return semPost(id);
}

int sys_semClose_handler(semID id)
{
    id += SEM_OFFSET;
    if (id + SEM_OFFSET < id)
        return -1;
    return semClose(id);
}

void sys_listProcess_handler()
{
    listProcesses();
}

void sys_listSemaphore_handler()
{
    semPrintAll();
}

PID sys_createprocess_handler(void *arg0, unsigned int arg1, char **arg2)
{
    return processCreate((void *)arg0, (unsigned int)arg1, (char **)arg2);
}

Background sys_getbackground_handler()
{
    return getBackground();
}

// Vector de punteros a funciones de llamada al sistema
static syscallT syscalls[] = {
    (syscallT)sys_read_handler,
    (syscallT)sys_write_handler,
    (syscallT)sys_time_handler,
    (syscallT)sys_setTerminalRawMode_handler,
    (syscallT)sys_font_handler,
    (syscallT)sys_printColor_handler,
    (syscallT)sys_clear_screen_handler,
    (syscallT)sys_screenData_handler,
    (syscallT)sys_paint_rect_handler,
    (syscallT)sys_ticks_handler,
    (syscallT)sys_beeper_handler,
    (syscallT)sys_memstate_handler,
    (syscallT)sys_alloc_handler,
    (syscallT)sys_free_handler,
    (syscallT)sys_exit_handler,
    (syscallT)sys_getPid_handler,
    (syscallT)sys_kill_handler,
    (syscallT)sys_changePriority_handler,
    (syscallT)sys_blockProcess_handler,
    (syscallT)sys_unblockProcess_handler,
    (syscallT)sys_yield_handler,
    (syscallT)sys_openPipe_handler,
    (syscallT)sys_close_handler,
    (syscallT)sys_mkFifo_handler,
    (syscallT)sys_openFifo_handler,
    (syscallT)sys_setBackground_handler,
    (syscallT)sys_mapStdFds_handler,
    (syscallT)sys_rmFifo_handler,
    (syscallT)sys_listPipes_handler,
    (syscallT)sys_listProcess_handler,
    (syscallT)sys_semOpen_handler,
    (syscallT)sys_semWait_handler,
    (syscallT)sys_semPost_handler,
    (syscallT)sys_semClose_handler,
    (syscallT)sys_listSemaphore_handler,
    (syscallT)sys_createprocess_handler,
    (syscallT)sys_getbackground_handler};

// Función para despachar llamadas al sistema
int64_t syscallDispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t rax)
{
    return syscalls[rax](rdi, rsi, rdx, rcx, r8); // rax tiene el id, rax = r9 y rcx = r10
}
