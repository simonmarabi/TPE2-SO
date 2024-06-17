// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <naiveGraphicsConsole.h>
#include <time.h>
#include <stdint.h>
#include <scheduler.h>

// Define el identificador de excepción para la división por cero.
#define ZERO_EXCEPTION_ID 0

// Define el identificador de excepción para una operación de código inválida.
#define INV_OP_EXC 6

// Define la cantidad de registros utilizados (incluyendo el registro de banderas).
#define REGISTERS 18

// Color para mostrar los mensajes de excepción en rojo.
#define RED 0x0000FF

// Declaración de la función de reinicio del sistema.
extern void restart();

// Nombres de los registros utilizados en el contexto de excepción.
static const char* registerNames[REGISTERS] = {
    "rip", "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8 ", "r9 ", "r10", "r11", "r12", "r13", "r14", "r15", "rflags"
};

// Mensaje de error genérico en caso de excepción.
static const char* errMsg = "The program aborted due to an exception: ";

// Nombres de excepciones específicas.
static const char* errNames[7] = {"Divide by Zero\n", "","","","","","Invalid Operation Code\n"};

//Convertion from int to hex of 16 chars (para que queden todos los registros de la misma longitud)
// Función para convertir un número entero en una cadena hexadecimal de 16 caracteres.
static void intToHex(uint64_t num, char buffer[17]){
	int i = 16;
	while (i-- != 0){
		int digit = num % 16;
        buffer[i] = (digit < 10 ? '0' : ('A' - 10)) + digit;
        num /= 16;
	}
}

// Función para imprimir los valores de los registros en la pantalla.
static void printRegisters(uint64_t reg[18]){
	char buf[19];
	buf[0] = '0';
	buf[1] = 'x';
	buf[18] = '\0';
	for (int i = 0; i < REGISTERS; i++){
		ngc_printColor(registerNames[i], RED);
		ngc_printColor(" : ", RED);
		intToHex(reg[i], buf + 2);
		ngc_printColor(buf, RED);
		ngc_printNewline();
	}
	
}

// Función que se encarga de manejar las excepciones y mostrar información en la pantalla.
void exceptionDispatcher(uint64_t exception, uint64_t regdata[18]) {
	ngc_printColor(errMsg, RED);
	ngc_printColor(errNames[exception], RED);
	printRegisters(regdata);
	terminateProcess(getPID());
	restart();
}