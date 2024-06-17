// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <syscalls.h>
#include <stdint.h>
#include <inout.h>
#include <stringUtil.h>
#include <stdarg.h>
#include <exceptions.h>
#include <inforeg.h>

#define MAX_BUFFER 255

// Función que pinta un rectángulo de un color específico en la pantalla.
void do_paintRect(uint64_t fromX, uint64_t fromY, uint16_t width, uint16_t height, uint64_t color){
    return sys_paint_rect(fromX, fromY, width, height, color);
}

// Función que produce un sonido (beep) en el sistema con una frecuencia y duración
void do_beep(uint64_t frequency, uint64_t nanoSeconds){
    sys_beeper(frequency, nanoSeconds);
}

// Función para imprimir texto con color
void do_printColor(const char * buffer, uint64_t color){
	sys_printColor(buffer, color);
}

// Función para cambiar el tamaño de fuente
void do_changeFontSize(uint64_t level){
	sys_changeFontSize(level);
}

// Función para escribir una cadena en la salida estándar
static void putString(const char *buffer){
    //lamo al syswrite 1=stdout
    sys_write(STDOUT, buffer, _strlen(buffer));
}

// Función para leer una línea de texto del teclado
void bufferAction(char * buffer, uint64_t length){
	int foundEnter = 0;
	if (length == 0){
		return;
	}
	char c;
	int i = 0;
	while(!foundEnter){
		
		c = do_getChar();
		// Si es enter entonces termina el string y sale 
		if (c == '\n' || c == '\0'){
			printf("\nENTER\n");
			buffer[i] = '\0';
			do_putChar(c);
			foundEnter = 1; 
		}
		// si no, se agrega al buffer
		else if(c >= 0){
			do_putChar(c);
			if (c == 0x7F && i >= 0){
				if (i > 0){
					i--;
				}
			}
			// validacion del maximo de scan
			else if (i < length-1){
				buffer[i] = c;
				i++;
			}    
		}
	}
	sys_clear_screen(0x000000);
}

// Función para obtener el formato de la hora
static uint64_t getFormat(uint64_t n) {
	uint64_t dec = n & 240;
	dec = dec >> 4;
	uint64_t units = n & 15;
	return dec * 10 + units;
}

// Función para obtener la hora actual del sistema
void do_getTime(char * buffer){
    char* p = buffer;
	uint64_t time = sys_time();
	itoa(getFormat(time & 0xFF), p);
	p[2] = ':';
	itoa(getFormat((time >> 8) & 0xFF), &p[3]);
	p[5] = ':';
	itoa(getFormat((time >> 16) & 0xFF), &p[6]);
    p[8] = 0;
}

// Función para manejar la excepción de división por cero
void do_divisionByZero(){
	exc_divisionByZero();
}

// Función para manejar la excepción de código de operación inválido
void do_invalidOpCode(){
	exc_invalidOpCode();
}

// Función para obtener un carácter del teclado
char do_getChar() {
    char out = '\0'; // Inicializamos out con un valor por defecto
    while (sys_read(STDIN, &out, 1) == 0);
    return out;
}


// Función scanf para leer entrada con formato variable
void scanf(char *format, ...) {
    // Lectura del buffer
    char buffer[MAX_BUFFER] = {0};
    if (sys_read(KBDIN, buffer, MAX_BUFFER) == -1) {
        return;
    }
    // El primer caracter debe ser %
    if (*format != '%') {
        printf("Uso incorrecto de scanf\n");
        return;
    }
    va_list vl;
    va_start(vl, format);
    int buffIdx = 0;
    format++; // Avanzamos al siguiente caracter después de %
    while (*format != '\0') {
        if (*format != '%') { // Es letra o espacio
            if (*format != ' ') {
                printf("Uso incorrecto de scanf\n");
                return;
            } else {
                format++; // Avanzamos al siguiente caracter
            }
        } else {
            format++; // Avanzamos al siguiente caracter después de %
            switch (*format) { // Caso en el que estoy en una letra 
                case 'd':
                case 'D':
                    *(int *)va_arg(vl, int *) = strtoi(buffer, &buffIdx);   
                    break;
                case 'c':
                case 'C':
                    *(char *)va_arg(vl, char *) = buffer[buffIdx++];
                    break;
                case ' ':
                    printf("Uso incorrecto de scanf (Fíjese de no dejar espacios luego del porcentaje)\n");
                    return;
            }
            format++; // Avanzamos al siguiente caracter después de %
        }
    }
    va_end(vl);
}


// Función para imprimir información de los registros
void do_printInfoReg(){
	inforeg_asm();
}

// Función para limpiar la pantalla
void do_clearScreen(uint64_t color) {
	sys_clear_screen(color);	
}

// Función para imprimir un carácter
void do_putChar(char c) {
	sys_write(STDOUT, &c, 1);
}

// Función para imprimir un número en base hexadecimal
void putBase(int num, int base){

	int i = 12;
	int j = 0;

	char hex[13];

	putString("0x");
	do{
		hex[i] = "0123456789ABCDEF"[num % base];
		i--;
		num = num/base;
	}while( i > 0 );

	while( ++i < 13){
		hex[j++] = hex[i];
	}
	hex[j] = 0;
	putString(hex);
}

// Función para imprimir un número entero
void putInt(int num){
	char strnum[MAX_INT];
	itoa(num,strnum);
	putString(strnum);
}

// Función printf para imprimir texto con formato
void printf (const char *format, ...) {
	// point p_arg to first argument
   	va_list p_arg;
   	va_start(p_arg, format); // se apunta p_arg al primer argumento (no format)

    while (*format != '\0') {
		// loop mientras se impriman char normales
        if (*format != '%') {
            do_putChar(*format);
            format++;
        } 
		// si hay un %, me fijo el siguiente para imprimir especial
		else {

        format++;

        switch (*format) {
            case 'd':
			case 'D':
                putInt(va_arg(p_arg, int));
                break;
            case 'c':
			case 'C':
                do_putChar(va_arg(p_arg, int));  // char promociona a int
                break;
            case 's':
			case 'S':
                putString(va_arg(p_arg, char *));
                break;
			case 'x':
            case 'X':
                putBase(va_arg(p_arg, int), 16);
				break;
			case 'l':
			case 'L':
				format++;
				if(*format == 'u' || *format == 'U')
					putBase(va_arg(p_arg, int), 16);
				break;
        }
        format++;
		}
    }
    va_end(p_arg);
}

// Función para imprimir el estado de la memoria
void do_printMem() {
	sys_printMem();	
}