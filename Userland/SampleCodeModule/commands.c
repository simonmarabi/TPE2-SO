#include <stdint.h>
#include <commands.h>
#include <snakes.h>
#include <colors.h>
#include <inout.h>
#include <syscalls.h>
#include <stringUtil.h>
#include <string.h>

// Función help (sin parámetros)
int help(int argc, const char* argv[])
{
	if(argc>1){
		printf("Be careful, help does not take any parameters\n");
	}
	// se corre el comando de todas formas
	printf("All Commands:\n");
	
	const char * helpstring = {
		
	"\thelp                 Provides help information for commands.\n"
    "\tdividebyzero         Command to verify the operation of the exception routine \"Divide by zero\"\n"
	"\tinvalidopcode        Command to verify the operation of the exception routine \"Invalid Opcode\"\n"
	"\tinforeg              Prints on screen the value of all registers.\n"
	"\ttime                 Command to display the system day and time.\n"
	"\tchangefontsize       Changes font size: insert 1 2 3 for the desired level.\n"
	"\tclear                Clear screen\n"
	"\tmem                  Prints on screen memory state\n"
	"\tkill                 Kills a Process\n"
	"\tloop                 Generates a Loop\n"
	"\tnice                 Changes process priority\n"
	"\tblock                Blocks a process\n"
	"\tcat                  Prints stdin as the same form as it receives\n"
	"\twc                   Counts input lines\n"
	"\tfilter               Filters the input vocals\n"
	"\tps                   Prints the list of the processes\n"
	"\tphylo                Implements the problem of the philosophers\n"
	"\ttest_mm              Test of memory manager\n"
	"\ttest_prio            Test of scheduler\n"
	"\ttest_processes       Test of process manager\n"
	"\ttest_sync            Test of syncronization\n"
							
	};
	
	printf(helpstring);

	return 0;
}

// Función para verificar la excepción de división por cero (sin parámetros)
int divideByZero(int argc, const char* argv[]){
	if(argc < 1){
		printf("\tTry divideByZero without parameters\n");
		return -1;
	}
	do_divisionByZero();
	return 0;
}

// Función para verificar la excepción de código de operación inválido (sin parámetros)
int invalidOPCode(int argc, const char* argv[]){
	if(argc<1){
		printf("\tTry invalidopcode without parameters\n");
		return -1;
	}
	do_invalidOpCode();
	return 0;
}

// Función para mostrar información de registros (sin parámetros)
int inforeg(int argc, const char* argv[]){
	if(argc<1){
		printf("\tTry inforeg without parameters\n");
		return -1;
	}
	do_printInfoReg();
	return 0;
}

// Función para cambiar el tamaño de fuente (1 parámetro: setting)
int changeFontSize(int argc, const char* argv[]){
	if(argc < 2){
		printf("\tTry change font size with 1 parameter (size)\n");
		return -1;
	}
	if (_strlen(argv[0]) > 1){
		printf("\tTry with 1, 2 or 3\n");
		return -1;
	}
	if (argv[0][0] == '1' || argv[0][0] == '2' || argv[0][0] == '3' ){
		do_changeFontSize(argv[0][0]-'0');
		return -1;
	}
	else{
		printf("\tInvalid parameter, try with 1, 2 or 3\n");
		return -1;
	}
	return 0;
}

// Función para mostrar la hora actual del sistema (sin parámetros)
int time(int argc, const char* argv[]){
	if(argc<1){
		printf("\tTry time without parameters\n");
		return -1;
	}
	printf("\tCurrent time in ");
	do_printColor("GREENWICH (UTC+00): ", ORANGE);
	printf("\n");
	char timeBuffer[9];
	do_getTime(timeBuffer);
	printf("\t\t\t");
	do_printColor(timeBuffer, CYAN);
	printf("\n");
	return 0;
}
//FALTA
int loop(){
	char timeBuffer[9];
	do_getTime(timeBuffer);
	PID pid = sys_getpid();
	int t, newT;
	strToIntBase(timeBuffer,_strlen(timeBuffer),10,&t,1);
	while(1){
		char newTimeBuffer[9];
		do_getTime(newTimeBuffer);
		strToIntBase(newTimeBuffer,_strlen(newTimeBuffer),10,&newT,1);
		if((t != newT) && (newT % 3 == 0)){
			t = newT;
			printf("[PID:%d] Hey I am Loop :3\n",pid);
		}
		sys_yield();
	}
}

int kill(int argc, const char* argv[]){
	
	if (argc < 2)
	{
		printf("USE: kill [PID]\n");
		return -1;
	}
	int pid = -1;
	strToIntBase(argv[1],_strlen(argv[1]),10,&pid,1);

	if (pid == -1)
	{
		printf("USE: kill [PID]\n");
		return -1;
	}
	if (sys_kill(pid)<0)
	{
		printf("Could not kill the process with PID %d\n");
		return -1;
	}
	return 0;
}

int nice(int argc, const char* argv[]){
	
	if (argc < 2)
	{
		printf("USE: nice [PID]\n");
		return -1;
	}

	int pid =-1;
	Priority priority = -1;
	strToIntBase(argv[1],_strlen(argv[1]),10,&pid,1);
	strToIntBase(argv[2],_strlen(argv[2]),10,(int*)&priority,1);

	if (pid == -1)
	{
		printf("USE: nice [PID] [Priority(1-3)]\n");
		return -1;
	}
	if (priority < 1 || priority > 3)
	{
		printf("USE: nice [PID] [Priority(1-3)]\n");
		return -1;
	}
	if (sys_chgpriority(pid,priority)<0)
	{
		printf("Could not change priority of process with PID %d\n");
		return -1;
	}
	return 0;
}

int block(int argc, const char* argv[]){

	if (argc < 2)
	{
		printf("USE: block [PID]\n");
		return -1;
	}
	int pid = -1;
	strToIntBase(argv[1],_strlen(argv[1]),10,&pid,1);

	if (pid == -1)
	{
		printf("USE: block [PID]\n");
		return -1;
	}
	if (sys_block(pid) < 0 && sys_unblock(pid)<0)
	{
		printf("Could not Block / Unbloked");
		return -1;
	}
	return 0;
}

int wc(){

	char buffer[1024];
	int bytesRead=0;
	int lines =1;
	int wasLastCharNewLine =0;
	while((bytesRead = sys_read(STDIN,buffer,1024))>0)
	{
		for (int i=0;i<bytesRead;i++)
		{
			wasLastCharNewLine=0;
			if (buffer[i] == '\n')
			{
				lines++;
				wasLastCharNewLine=0;
			}
			
		}
	}
	if (wasLastCharNewLine) lines--;
	printf("\nLine Count: %d\n");
	return 0;
}

// Función para iniciar el juego Snakes (sin parámetros)
int snakes(int argc, const char* argv[]){
	play_snakes();
	return 0;
}

// Función para limpiar la pantalla (sin parámetros)
int clearScreen(int argc, const char* argv[]) {
	do_clearScreen(BLACK);
	return 0;
}

// Función para obtener el estado de la memoria (sin parámetros)
int printMem(int argc, const char* argv[]){
	do_printMem();
	return 0;
}

int filter(){
	
	char buffer[1024];
    int bytesRead = 0;
    while((bytesRead = sys_read(STDIN, buffer, 1024)) > 0) {
        for(int i = 0; i < bytesRead; i++) {
            if( buffer[i] == 'a' ||
                buffer[i] == 'e' ||
                buffer[i] == 'i' ||
                buffer[i] == 'o' ||
                buffer[i] == 'u') continue;
                
            sys_write(1, &buffer[i], 1);
        }
    }

    return 0;
}

int cat() {
    char buffer[1024];
    int bytesRead = 0;
    while((bytesRead = sys_read(STDIN, buffer, 1024)) > 0) {
        sys_write(1, buffer, bytesRead);
    }

    return 0;
}

int ps(){
	sys_listprocesses();
    return 0;
}

int printcmd(){
	printf("%s",cmds[0].name);
	return 0;
}



