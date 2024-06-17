// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
uint64_t itoa(uint64_t number, char *s)
{
	int digits = 1;
	for (int n = number / 10; n != 0; digits++, n /= 10)
		;

	if (digits == 1)
	{
		s[0] = '0';
		s[1] = number + '0';
		s[2] = 0;
		return digits;
	}

	s[digits] = 0;
	for (int i = digits - 1; i >= 0; i--)
	{
		s[i] = (number % 10) + '0';
		number /= 10;
	}

	return digits;
}

// Función para copiar una cadena de origen en una cadena de destino.
void _strcpy(char *dest, char *src, int n)
{
	while (*src != '\0' && n > 0)
	{
		*dest++ = *src++;
		n--;
	}
	*dest = '\0';
}

// Función para obtener la longitud de una cadena de caracteres.
uint64_t _strlen(const char *str)
{
	uint64_t ans = 0;
	while (str[ans] != '\0')
	{
		ans++;
	}
	return ans;
}

// Función para comparar dos cadenas de caracteres.
int _strcmp(const char *str1, const char *str2)
{
	while ((*str1 != '\0' && *str2 != '\0') && *str1 == *str2)
	{
		str1++;
		str2++;
	}
	if (*str1 == *str2)
	{
		return 0;
	}
	else
	{
		return *str1 - *str2;
	}
}

// Función para convertir una subcadena en un número entero.
int strtoi(char *buffer, int *i)
{
	char strnum[MAX_INT];
	int numsize = 0;
	while (buffer[*i] != ' ' && buffer[*i] != '\n' && buffer[*i] != '\0')
	{
		strnum[numsize++] = buffer[*i];
		(*i)++;
	}
	uint64_t out = atoi(strnum);
	return out;
}

uint32_t uintToBase(uint32_t value, char *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	*p = 0;

	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

// int64_t strToIntBase(const char * s, int len, uint32_t base, int * result, int unsign) {
// 	int sign = 1;
// 	uint8_t aux = 0;
// 	uint32_t num = 0;
// 	int i;
// 	if (!unsign && s[0] == '-') {
//         s++;
//     	sign = -1;
//     }
//     for (i = 0; s[i] != 0 && ((len < 0)? 1 : i < len) ; i++)
//     {
//         if (s[i] <= '9') {
// 			aux = s[i] - '0';
// 		}
// 		else
// 			aux = toUpper(s[i]) + 10 - 'A';

// 		if (aux >= base)
// 		{
// 			break;
// 		}

// 		num *= base;
// 		num += aux;
// 	}
// 	num *= sign;
// 	if (i != 0)
// 	{
// 		*result = num;
// 	}
// 	return sign == 1? i : i + 1;
// }

int64_t strToIntBase(const char *s, int len, uint32_t base, int *result, int unsign)
{
	int sign = 1;
	uint8_t aux = 0;
	uint32_t num = 0;
	int i;

	// Manejo del signo si es necesario
	if (!unsign && s[0] == '-')
	{
		s++;
		sign = -1;
	}

	// Iteración sobre la cadena s
	for (i = 0; s[i] != 0; i++)
	{
		// Verificación de longitud máxima si len >= 0
		if (len >= 0 && i >= len)
		{
			break; // Salir si se alcanza la longitud máxima especificada
		}

		// Conversión de caracteres a valores numéricos
		if (s[i] <= '9')
		{
			aux = s[i] - '0';
		}
		else
		{
			aux = toUpper(s[i]) + 10 - 'A';
		}

		// Verificación de rango para la base especificada
		if (aux >= base)
		{
			break; // Salir si el caracter no es válido para la base
		}

		num *= base;
		num += aux;
	}

	num *= sign;

	// Almacenar el resultado si se leyó al menos un caracter válido
	if (i != 0)
	{
		*result = num;
	}

	return sign == 1 ? i : i + 1;
}

uint8_t toUpper(uint8_t c)
{
	return c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c;
}
