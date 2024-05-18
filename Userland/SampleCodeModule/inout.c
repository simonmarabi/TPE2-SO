#include <syscalls.h>
#include <stdint.h>
#include <inout.h>
#include <stringUtil.h>
#include <stdarg.h>
#include <exceptions.h>

#define MAX_BUFFER 255

// Función que pinta un rectángulo de un color específico en la pantalla.
void do_paintRect(uint64_t fromX, uint64_t fromY, uint16_t width, uint16_t height, uint64_t color){
    return sys_paint_rect(fromX, fromY, height, width, color);
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
		if (c == '\n'){
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
char do_getChar(){
	char out;
    while(sys_read(STDIN, &out, 1) == 0);
    return out;
}

// Función scanf para leer entrada con formato variable
void scanf(char * format,...){
	//lectura del buffer
	char buffer[MAX_BUFFER] = {0};
	if(sys_read(KBDIN, buffer, MAX_BUFFER) == -1) {
		return;
	}
	// el primer caracter debe ser %
	if(*format != '%'){
		printf("uso incorrecto de scanf\n");
		return;
	}
	va_list vl;
	va_start(vl, format);
	int buffIdx = 0;
	while (*format != '\0'){
		if(*format != '%'){ // es letra o espacio
			if ( *format != ' '){
				printf("uso incorrecto de scanf\n");
				return;
			}
			else{
				(*format)++; 
			}
		}
		else{
			(*format)++;
			switch (*format) { // caso en el que estoy en una letra 
            	case 'd':
				case 'D':
					*(int *)va_arg( vl, int* ) = strtoi(buffer, &buffIdx);	
                	break;
            	case 'c':
				case 'C':
					*(char *)va_arg( vl, char* ) = buffer[buffIdx++];
                	break;
				case ' ':
					printf("uso incorrecto de scanf (Fijese de no dejar espacios luego del porcentaje)\n");
					return;
			}
			(*format)++;	
		}
	}
	va_end(vl);
}

// Función para imprimir información de los registros
void do_printInfoReg(){
	uint64_t reg[17];
	uint8_t huboScreenshot = sys_inforeg(reg);
	if (huboScreenshot){
		//rip, rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15 
		printf("rip = %x\n", reg[0]);
		printf("rax = %x\n", reg[1]);
		printf("rbx = %x\n", reg[2]);
		printf("rcx = %x\n", reg[3]);
		printf("rdx = %x\n", reg[4]);
		printf("rsi = %x\n", reg[5]);
		printf("rdi = %x\n", reg[6]);
		printf("rbp = %x\n", reg[7]);
		printf("rsp = %x\n", reg[8]);
		printf("r8  = %x\n", reg[9]);
		printf("r9  = %x\n", reg[10]);
		printf("r10 = %x\n", reg[11]);
		printf("r11 = %x\n", reg[12]);
		printf("r12 = %x\n", reg[13]);
		printf("r13 = %x\n", reg[14]);
		printf("r14 = %x\n", reg[15]);
		printf("r15 = %x\n", reg[16]);
	} else {
		printf("\tA screenshot was not made, press CTRL and try again.\n");
	}
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
        }
        format++;
		}
    }
    va_end(p_arg);
}
