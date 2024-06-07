#include <stdint.h>
#include <commands.h>
#include <snakes.h>
#include <colors.h>
#include <inout.h>
#include <syscalls.h>
#include <stringUtil.h>
#include <string.h>

// Función help (sin parámetros)
int help(int argc, char params[MAX_PARAMETERS][LENGTH_PARAMETERS])
{
	if(argc!=0){
		printf("Be careful, help does not take any parameters\n");
	}
	// se corre el comando de todas formas
	printf("All Commands\n");
	for (int i=0; i <= REGISTERS;i++)
	{
		printf("%s\n",cmds[i].name);
	}
	printf("Wiki\n");
	const char* helpstring = 
	"\thelp                 Provides help information for commands.\n"
    "\tdividebyzero         Command to verify the operation of the exception routine \"Divide by zero\"\n"
	"\tinvalidopcode        Command to verify the operation of the exception routine \"Invalid Opcode\"\n"
	"\tinforeg              Prints on screen the value of all registers.\n"
	"\ttime                 Command to display the system day and time.\n"
	"\tchangefontsize       Changes font size: insert 1 2 3 for the desired level.\n"
	"\tsnakes               Get ready to play Snakes!.\n"
	"\tclear                Clear screen\n"
	"\tmem                  Prints on screen memory state\n";
	printf(helpstring);

	return 0;
}

// Función para verificar la excepción de división por cero (sin parámetros)
int divideByZero(int argc, char  params[][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("\tTry divideByZero without parameters\n");
		return -1;
	}
	do_divisionByZero();
	return 0;
}



// Función para verificar la excepción de código de operación inválido (sin parámetros)
int invalidOPCode(int argc, char  params[][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("\tTry invalidopcode without parameters\n");
		return -1;
	}
	do_invalidOpCode();
	return 0;
}

// Función para mostrar información de registros (sin parámetros)
int inforeg(int argc, char params[][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("\tTry inforeg without parameters\n");
		return -1;
	}
	do_printInfoReg();
	return 0;
}

// Función para cambiar el tamaño de fuente (1 parámetro: setting)
int changeFontSize(int argc, char params[][LENGTH_PARAMETERS]){
	if(argc != 1){
		printf("\tTry change font size with 1 parameter (size)\n");
		return -1;
	}
	if (_strlen(params[0]) > 1){
		printf("\tTry with 1, 2 or 3\n");
		return -1;
	}
	if (params[0][0] == '1' || params[0][0] == '2' || params[0][0] == '3' ){
		do_changeFontSize(params[0][0]-'0');
		return -1;
	}
	else{
		printf("\tInvalid parameter, try with 1, 2 or 3\n");
		return -1;
	}
	return 0;
}

// Función para mostrar la hora actual del sistema (sin parámetros)
int time(int argc, char params[][LENGTH_PARAMETERS]){
	if(argc!=0){
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

	while(1){

		char newTimeBuffer[9];
		do_getTime(newTimeBuffer);
		printf("[PID:%d] Hey I am Loop :3",pid);
	}
}

int kill(int argc, char argv[][LENGTH_PARAMETERS]){
	
	if (argc <2)
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

int nice(int argc, char argv[][LENGTH_PARAMETERS]){
	
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

int block(int argc, char argv[][LENGTH_PARAMETERS]){

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
int snakes(int argc, char argv[][LENGTH_PARAMETERS]){
	play_snakes();
	return 0;
}

// Función para limpiar la pantalla (sin parámetros)
int clearScreen(int argc, char params[][LENGTH_PARAMETERS]) {
	do_clearScreen(BLACK);
	return 0;
}

// Función para obtener el estado de la memoria (sin parámetros)
int printMem(int argc, char params[][LENGTH_PARAMETERS]){
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

int pipe() {
    sys_listpipes();
    return 0;
}

int sem() {
    sys_listsem();
    return 0;
}


