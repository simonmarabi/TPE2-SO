// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <stdint.h>
#include <stringUtil.h>
#include <time.h>
#include <inout.h>

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define DEAULT_PHILO_COUNT 5
#define MAX_PHILO_COUNT 32
#define MUTEX_ID 128
#define SEMAPHORE_MIN_ID 129

#define LEFT(x) (((x) + philoCount - 1) % philoCount)
#define RIGHT(x) (((x) + 1) % philoCount)

static uint8_t philoVector[MAX_PHILO_COUNT];
static int philoPID[MAX_PHILO_COUNT];
static int philoCount;

void tryToEat(int idx)
{
    philoVector[idx] = HUNGRY;
    while (philoVector[idx] == HUNGRY)
    {
        sys_semwait(MUTEX_ID);
        if (philoVector[idx] == HUNGRY && philoVector[LEFT(idx)] != EATING && philoVector[RIGHT(idx)] != EATING)
        {
            philoVector[idx] = EATING;
            sys_sempost(SEMAPHORE_MIN_ID + idx);
        }
        sys_sempost(MUTEX_ID);
        sys_semwait(SEMAPHORE_MIN_ID + idx);
    }
}

void stopEating(int idx)
{
    sys_semwait(MUTEX_ID);
    philoVector[idx] = THINKING;
    if (philoVector[LEFT(idx)] == HUNGRY)
        sys_sempost(SEMAPHORE_MIN_ID + LEFT(idx));
    if (philoVector[RIGHT(idx)] == HUNGRY)
        sys_sempost(SEMAPHORE_MIN_ID + RIGHT(idx));
    sys_sempost(MUTEX_ID);
}

void philosophers(unsigned int argc, const char **argv)
{
    int idx = 0; // Valor por defecto para idx

    if (argc > 1)
        strToIntBase(argv[1], _strlen(argv[1]), 10, &idx, 1);

    // MEJOR CORTARLO CON CNTRL C
    int times = 10;
    while (times--)
    {
        // Thinking
        tryToEat(idx);
        // Eating
        stopEating(idx);
    }
}

void philosopherPrinter()
{
    uint64_t t = sys_time();
    while (1)
    {
        uint64_t newT = sys_time();
        if (t != newT)
        {
            t = newT;
            for (int i = 0; i < philoCount; i++)
            {
                char *printState;
                switch (philoVector[i])
                {
                case EATING:
                    printState = "E";
                    break;
                default:
                    printState = ".";
                    break;
                }

                printf("%s ", printState);
            }
            do_putChar('\n');
        }
    }
}

void philosopherManager()
{
    sys_setTerminalRawMode(1);

    philoCount = DEAULT_PHILO_COUNT;

    while (1)
    {
        int printerpid;
        int modified = 0;
        int end = 0;

        sys_semopen(MUTEX_ID, 1);

        char idstr[10];
        char *argv[] = {"philosopher", idstr, 0};

        for (int i = 0; i < philoCount; i++)
        {
            uintToBase(i, idstr, 10);
            sys_semopen(SEMAPHORE_MIN_ID + i, 0);
            philoPID[i] = sys_createprocess(&philosophers, 2, argv);
        }

        char *printerArgs[] = {"phprinter", 0};
        printerpid = sys_createprocess(&philosopherPrinter, 1, printerArgs);

        char buf[2] = {0}; // Inicialización del arreglo buf

        while (sys_read(0, buf, 1))
        {
            switch (buf[0])
            {
            case 'a':
                if (philoCount == MAX_PHILO_COUNT)
                    continue;
                modified = 1;
                break;
            case 'r':
                if (philoCount == 2)
                    continue;
                modified = -1;
                break;
            case 't':
                end = 1;
                break;
            default:
                continue;
            }
            break;
        }

        sys_kill(printerpid);
        for (int i = 0; i < philoCount; i++)
        {
            sys_kill(philoPID[i]);
            sys_semclose(SEMAPHORE_MIN_ID + i);
        }
        sys_semclose(MUTEX_ID);

        if (end)
            break;

        philoCount += modified;
    }

    sys_setTerminalRawMode(0);
}
