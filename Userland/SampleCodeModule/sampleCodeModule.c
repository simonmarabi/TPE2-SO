// #include <inout.h>
// #include <commands.h>
// #include <stdint.h>
// #include <stringUtil.h>
// #include <snakes.h>
// #include <syscalls.h>
// #include <colors.h>

// // Arreglo de comandos disponibles
// static char* commands[] = {"help", "invalidopcode", "dividebyzero", "inforeg", "time", "changefontsize", "snakes", "clear","mem"};

// // Declaración de funciones de comandos disponibles
// static void (*commands_functions[])(int argc, char parameters[MAX_PARAMETERS][LENGTH_PARAMETERS]) = {
// 	help, 
// 	invalidOPCode, 
// 	divideByZero, 
// 	inforeg,  
// 	time, 
// 	changeFontSize, 
// 	snakes, 
// 	clearScreen,
// 	printMem
// };

// // Función para encontrar el índice de un comando en el arreglo de comandos
// static int findIdxCommand(char *buff);

// // Función para analizar el buffer de entrada en comandos y parámetros
// static int parseBuffer(char command[BUFFER_LENGTH], char parameters[MAX_PARAMETERS][LENGTH_PARAMETERS], char readbuf[BUFFER_LENGTH]);

// int main() {
// 	do_printColor("WELCOME! Which module do you want to start with? Type \"help\" for command list\n", WHITE);
// 	while(1){
// 			printf("$>");
// 			char buff_command[BUFFER_LENGTH] = {0};
// 			// command
// 			char command[BUFFER_LENGTH] = {0};
// 			// parametros enviados junto al comando
// 			char parameters[MAX_PARAMETERS][LENGTH_PARAMETERS] = {{0}};

// 			bufferAction(buff_command, BUFFER_LENGTH); //sys_read de todo
// 			int size = parseBuffer(command, parameters, buff_command);
// 			int idx = findIdxCommand(command);
// 			if(idx >= 0 ){
// 				commands_functions[idx](size,parameters);
// 			}
// 			else if (idx == -1){
// 				do_printColor("Command not found: try again\n", RED);
// 			}
// 	}
// 	return 0;
// }

// // Función para analizar el buffer de entrada en comandos y parámetros


// // Función para encontrar el índice de un comando en el arreglo de comandos
// static int findIdxCommand(char *buff){

// 	if (_strlen(buff) == 0) { // Caso enter suelto
// 		return -2;            
// 	}
	
// 	for (int i = 0; i < COMMANDS_LENGTH ; i++){
// 		if (_strcmp(buff, commands[i]) == 0){
// 			return i;
// 		}
// 	}
// 	return -1; //command not found
// }
#include <shell.h>

int main()
{
	runShell();
	return 0;
}