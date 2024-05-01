/* _loader.c */
#include <stdint.h>

// Declaración de variables externas para marcar el inicio de la sección BSS y el final del binario.
extern char bss;
extern char endOfBinary;

// Declaración de la función principal.
int main();

// Función que llena un bloque de memoria con un valor dado.
void * memset(void * destiny, int32_t c, uint64_t length);

// Punto de entrada del programa.
int _start() {
	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);

	return main();

}

// Función que llena un bloque de memoria con un valor dado.
void * memset(void * destiation, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destiation;

	while(length--)
		dst[length] = chr;

	return destiation;
}
