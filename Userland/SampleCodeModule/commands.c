#include <stdint.h>
#include <commands.h>
#include <snakes.h>
#include <colors.h>
#include <inout.h>
#include <stringUtil.h>

// Función help (sin parámetros)
void help(int argc, char params[MAX_PARAMETERS][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("Be careful, help does not take any parameters\n");
	}
	// se corre el comando de todas formas
	
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
}

// Función para verificar la excepción de división por cero (sin parámetros)
void divideByZero(int argc, char  params[][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("\tTry divideByZero without parameters\n");
		return;
	}
	do_divisionByZero();
}

// Función para verificar la excepción de código de operación inválido (sin parámetros)
void invalidOPCode(int argc, char  params[][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("\tTry invalidopcode without parameters\n");
		return;
	}
	do_invalidOpCode();
}

// Función para mostrar información de registros (sin parámetros)
void inforeg(int argc, char params[][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("\tTry inforeg without parameters\n");
		return;
	}
	do_printInfoReg();
	return;
}

// Función para cambiar el tamaño de fuente (1 parámetro: setting)
void changeFontSize(int argc, char params[][LENGTH_PARAMETERS]){
	if(argc != 1){
		printf("\tTry change font size with 1 parameter (size)\n");
		return;
	}
	if (_strlen(params[0]) > 1){
		printf("\tTry with 1, 2 or 3\n");
		return;
	}
	if (params[0][0] == '1' || params[0][0] == '2' || params[0][0] == '3' ){
		do_changeFontSize(params[0][0]-'0');
		
	}
	else{
		printf("\tInvalid parameter, try with 1, 2 or 3\n");
	}
	return;
}

// Función para mostrar la hora actual del sistema (sin parámetros)
void time(int argc, char params[][LENGTH_PARAMETERS]){
	if(argc!=0){
		printf("\tTry time without parameters\n");
		return;
	}
	printf("\tCurrent time in ");
	do_printColor("GREENWICH (UTC+00): ", ORANGE);
	printf("\n");
	char timeBuffer[9];
	do_getTime(timeBuffer);
	printf("\t\t\t");
	do_printColor(timeBuffer, CYAN);
	printf("\n");
	return;
}

// Función para iniciar el juego Snakes (sin parámetros)
void snakes(int argc, char params[][LENGTH_PARAMETERS]){
	play_snakes();
	return;
}

// Función para limpiar la pantalla (sin parámetros)
void clearScreen(int argc, char params[][LENGTH_PARAMETERS]) {
	do_clearScreen(BLACK);
	return;
}

// Función para obtener el estado de la memoria (sin parámetros)
void printMem(int argc, char params[][LENGTH_PARAMETERS]){
	do_printMem();
	return;
}

