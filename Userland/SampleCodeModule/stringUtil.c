#include <stringUtil.h>
#include <inout.h>
#include <stdint.h>

// Función para convertir una cadena de caracteres en un entero sin signo de 64 bits.
uint64_t atoi(char S[])
{
    uint64_t num = 0;
    int i = 0;
    // sigue mientras que no se llego al final del string o el caracter es no numerico
    while (S[i] && (S[i] >= '0' && S[i] <= '9'))
    {
        num = num * 10 + (S[i] - '0');
        i++;
    }
    return num;
}

// Función para convertir un número en un formato de cadena de caracteres.
uint64_t itoa(uint64_t number, char* s) {
    int digits = 1;
	for (int n=number/10; n != 0; digits++, n /= 10);

    if(digits == 1) {
        s[0] = '0';
        s[1] = number + '0';
        s[2] = 0;
        return digits;
    }
	
	s[digits] = 0;    
	for (int i=digits-1; i>=0; i--) {
		s[i] = (number % 10) + '0';
		number /= 10;
	}
	
	return digits;
}

// Función para copiar una cadena de origen en una cadena de destino.
void _strcpy(char * dest, char * src, int n){
    while (*src != '\0' && n > 0){
        *dest++ = *src++;
        n--;
    }
    *dest = '\0';
}

// Función para obtener la longitud de una cadena de caracteres.
uint64_t _strlen(const char * str){
	uint64_t ans = 0;
    while(str[ans] != '\0'){
        ans++;
    }
	return ans;
}

// Función para comparar dos cadenas de caracteres.
int _strcmp(char *str1, char *str2){
	while( ( *str1 != '\0' && *str2 != '\0' ) && *str1 == *str2 )
    {
        str1++;
        str2++;
    }
    if(*str1 == *str2){
        return 0;
    }
    else{
        return *str1 - *str2;
    }
}

// Función para convertir una subcadena en un número entero.
int strtoi(char * buffer, int * i){
	char strnum[MAX_INT];
	int numsize=0;
	while(buffer[*i] != ' ' && buffer[*i] != '\n' && buffer[*i] != '\0'){
		strnum[numsize++] = buffer[*i];
		(*i)++;
	}
	uint64_t out = atoi(strnum);
	return out;
}
