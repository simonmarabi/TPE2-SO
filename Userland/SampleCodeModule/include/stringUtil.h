#ifndef STRING_H_
#define STRING_H_

#include <stdint.h>

#define MAX_INT 18

void _strcpy(char * dest, char * src, int n);

uint64_t _strlen(const char  *str);

int _strcmp(const char *str1,const char *str2);

uint64_t itoa(uint64_t number, char* s);

int strtoi(char * buffer, int * i);

uint64_t atoi(char S[]);

uint32_t uintToBase(uint32_t value, char * buffer, uint32_t base);

int64_t strToIntBase(const char * s, int len, uint32_t base, int * result, int unsign);

uint8_t toUpper(uint8_t c);

#endif