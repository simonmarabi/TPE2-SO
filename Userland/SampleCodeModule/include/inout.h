#ifndef INOUT_H
#define INOUT_H

#include <stdint.h>

void do_paintRect(uint64_t fromX, uint64_t fromY, uint16_t width, uint16_t height, uint64_t color);

void do_beep(uint64_t frequency, uint64_t nanoSeconds);

void bufferAction(char * buffer, uint64_t length);

void printf(const char *format, ...);

void scanf(char* format, ...);

void do_printColor(const char * buffer, uint64_t color);

void do_getTime(char * buffer);

void do_clearScreen(uint64_t color);

void do_divisionByZero();

void do_invalidOpCode();

void do_printInfoReg();

void do_changeFontSize(uint64_t level);

char do_getChar();

void do_putChar(char c);

void do_printMem();

#endif