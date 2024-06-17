// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#include <inout.h>
#include <stringUtil.h>
#include <commands.h>
#include <syscalls.h>

#define NULL 0

#define MAX_CMD_LEN 1024

#define START_PROC_SEM 1

#define WRAPPER_ARGS 3

static PID shellPid;
static PID forePid;

static void setShellBackground(){
    sys_setbackground(shellPid, BACKGROUND);
}

static const Command parseCommand(int argc, const char ** argv){
    for(int i = 0; i < CMD_COUNT; i++){
        if(_strcmp(argv[0], cmds[i].name) != NULL) continue;
        Command c = cmds[i];
        c.isBackground = argv[argc - 1][0] == '&';
        return c;
    }

    Command c = {NULL, NULL, 0};
    return c;
}

static uint8_t split(char * input, char * buf[], uint8_t maxCount){

    char c;
    int word = 0;
    buf[word] = input;

    while((c = *input++) != 0 && word < maxCount-1){
        if(c != ' ') continue;

        *(input-1) = 0;
        buf[++word] = input;
    }

    return word + 1;

}

int readInput(char *outputBuffer)
{
    int len = 0;
    if(sys_getbackground() != BACKGROUND){
        printf(">$ ");
        while((len = sys_read(0,outputBuffer,MAX_CMD_LEN)) <= 0);
        outputBuffer[len-1] = 0;
        return 0;  
    }
    else{
        while((len = sys_read(0,outputBuffer,MAX_CMD_LEN)) <= 0);
        if (_strcmp(outputBuffer, "kill") == 0)
            if(sys_kill(forePid) != -1)
                printf("Process %d terminated correctly\n", forePid);
        _strcpy(outputBuffer,"",1);
        return 0;
    }
    return -1;   
}

int shellProcessWrapper(int argc, char **argv)
{
    sys_semopen(START_PROC_SEM,0);
    sys_semwait(START_PROC_SEM);
    
    Background bg;
    strToIntBase(argv[argc-WRAPPER_ARGS+1], _strlen(argv[argc-WRAPPER_ARGS+1]), 10, (int*)&bg, 1);

    uint64_t cmdInt;
    strToIntBase(argv[argc-WRAPPER_ARGS], _strlen(argv[argc-WRAPPER_ARGS]), 10, (int*)&cmdInt, 1);
    
    CmdHandler cmd = (CmdHandler)cmdInt;

    int unusedPipeEnd;
    strToIntBase(argv[argc-WRAPPER_ARGS+2], _strlen(argv[argc-WRAPPER_ARGS+2]), 10, &unusedPipeEnd, 0);

    if (unusedPipeEnd >=0 )
    {
        sys_close(unusedPipeEnd);
    }

    PID pid = sys_getpid();
    int retcode;
    if(argv[argc-WRAPPER_ARGS-1][0] == '&')
        retcode = cmd(argc-WRAPPER_ARGS-1, (const char**) argv);
    else
        retcode = cmd(argc-WRAPPER_ARGS, (const char**) argv);
    
    sys_mapstdfds(pid,0,1);

    sys_semclose(START_PROC_SEM);
    sys_exit();
    return retcode;
}

int checkPipeCommand(int * argc0, char ** argv0, char ** argv1){
    int totalArgs = *argc0;
    for(int i = 0; i < totalArgs; i++){
        if(_strcmp(argv0[i], "|") == 0){
            *argc0 = i;

            for(int j = i+1; j < totalArgs; j++){
                argv1[j-i-1] = argv0[j];
            }

            return 1;
        }
    }
    
    return 0;
}

void runCommand(Command cmd, int argc, char** argv)
{
    char handlerStr[24];
    uintToBase((uint64_t)cmd.handler, handlerStr, 10);

    argv[argc] = handlerStr;
    argv[argc+2] = "-1";

    if(cmd.isBackground)
    {
        argv[argc+1] = "1";
        PID pid = sys_createprocess(&shellProcessWrapper, argc + WRAPPER_ARGS, argv);
        sys_chgpriority(pid, 1);
        sys_sempost(START_PROC_SEM); 
    }
    else {
        argv[argc+1] = "0";
        PID pid = sys_createprocess(&shellProcessWrapper, argc + WRAPPER_ARGS, argv);
        setShellBackground();
        sys_setbackground(pid, FOREGROUND);
        forePid = pid;
        sys_sempost(START_PROC_SEM);
    }
}

// void runPipedCommands(Command cmd0, Command cmd1, int argc0, int argc1, char** argv0, char** argv1 ){
//     uint64_t pipe[2];
//     if(sys_pipe(pipe) < 0){
//         printf("[SHELL] ERROR: PIPE FAILED\n");
//         return;
//     }
//     char handler0Str[24]; char handler1Str[24];
//     uintToBase((uint64_t)cmd0.handler, handler0Str, 10);
//     uintToBase((uint64_t)cmd1.handler, handler1Str, 10);

//     char pipeOutStr[24]; char pipeInStr[24];
//     uintToBase(pipe[1], pipeOutStr, 10);
//     uintToBase(pipe[0], pipeInStr, 10);

//     argv0[argc0] = handler0Str;
//     argv1[argc1] = handler1Str;

//     argv0[argc0+2] = pipeInStr;
//     argv1[argc1+2] = pipeOutStr;

//     if(cmd1.isBackground){
//         argv0[argc0+1] = "1";
//         argv1[argc1+1] = "1";
//     }
//     else{
//         argv0[argc0+1] = "0";
//         argv1[argc1+1] = "0";
//     }

//     PID pid0 = sys_createprocess(&shellProcessWrapper, argc0+WRAPPER_ARGS, argv0);
//     PID pid1 = sys_createprocess(&shellProcessWrapper, argc1+WRAPPER_ARGS, argv1);
//     sys_mapstdfds(pid0, STDIN, pipe[1]);
//     sys_mapstdfds(pid1, pipe[0],STDOUT);
//     sys_close(pipe[0]);
//     sys_close(pipe[1]);

//     if(cmd1.isBackground){
//         sys_chgpriority(pid0, 1);
//         sys_chgpriority(pid1, 1);
//         sys_sempost(START_PROC_SEM);
//         sys_sempost(START_PROC_SEM);
//     }
//     else{
//         setShellBackground();
//         sys_setbackground(pid0, FOREGROUND);
//         forePid = pid0;
//         sys_sempost(START_PROC_SEM);
//         sys_sempost(START_PROC_SEM);
//     }
// }

void runPipedCommands(Command cmd0, Command cmd1, int argc0, int argc1, char** argv0, char** argv1) {
    uint64_t pipe[2];
    if (sys_pipe(pipe) < 0) {
        printf("[SHELL] ERROR: PIPE FAILED\n");
        return;
    }

    // Variables estáticas para almacenar las cadenas convertidas
    static char handler0Str[24];
    static char handler1Str[24];
    static char pipeOutStr[24];
    static char pipeInStr[24];

    // Convertir valores a cadenas estáticas
    uintToBase((uint64_t) cmd0.handler, handler0Str, 10);
    uintToBase((uint64_t) cmd1.handler, handler1Str, 10);
    uintToBase(pipe[1], pipeOutStr, 10);
    uintToBase(pipe[0], pipeInStr, 10);

    // Asignar punteros a argv0 y argv1
    argv0[argc0] = handler0Str;
    argv1[argc1] = handler1Str;
    argv0[argc0 + 2] = pipeInStr;
    argv1[argc1 + 2] = pipeOutStr;

    // Configurar otros argumentos según sea necesario
    if (cmd1.isBackground) {
        argv0[argc0 + 1] = "1";
        argv1[argc1 + 1] = "1";
    } else {
        argv0[argc0 + 1] = "0";
        argv1[argc1 + 1] = "0";
    }

    // Crear procesos y manejar descriptores de archivos
    PID pid0 = sys_createprocess(&shellProcessWrapper, argc0 + WRAPPER_ARGS, argv0);
    PID pid1 = sys_createprocess(&shellProcessWrapper, argc1 + WRAPPER_ARGS, argv1);
    sys_mapstdfds(pid0, STDIN, pipe[1]);
    sys_mapstdfds(pid1, pipe[0], STDOUT);
    sys_close(pipe[0]);
    sys_close(pipe[1]);

    // Manejar prioridades y semáforos según sea necesario
    if (cmd1.isBackground) {
        sys_chgpriority(pid0, 1);
        sys_chgpriority(pid1, 1);
        sys_sempost(START_PROC_SEM);
        sys_sempost(START_PROC_SEM);
    } else {
        setShellBackground();
        sys_setbackground(pid0, FOREGROUND);
        forePid = pid0;
        sys_sempost(START_PROC_SEM);
        sys_sempost(START_PROC_SEM);
    }
}


void runShell(){
    sys_semopen (START_PROC_SEM, 0);
    shellPid = sys_getpid();
    
    while(1)
    {
        char input[MAX_CMD_LEN];
        
        if(readInput(input) < 0)
			continue;

        if(_strcmp(input, "") == 0)
            continue;
            
        char* argv0[64];
        int totalArgc = split(input, argv0, 64);
        
        int argc0 = totalArgc;
        char* argv1[64];
        int pipeCommand = checkPipeCommand(&argc0, argv0, argv1);
        
        if(pipeCommand){
            int argc1 = totalArgc - argc0 - 1;
            Command cmd0 = parseCommand(argc0, (const char**)argv0);
            Command cmd1 = parseCommand(argc1, (const char**)argv1);

            if(cmd0.handler == NULL || cmd1.handler == NULL){
                printf("[SHELL] Error: command not found\n");
                continue;
            }

            runPipedCommands(cmd0, cmd1, argc0, argc1, argv0, argv1);
            continue;
        }
        Command cmd = parseCommand(totalArgc, (const char**)argv0);
        if(cmd.handler == NULL) {
            printf("Unknown command: %s\n", argv0[0]);
            continue;
        }
        runCommand(cmd, totalArgc, argv0);
        
    }

    sys_semclose(START_PROC_SEM);
    
}
