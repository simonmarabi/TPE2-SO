#pragma once
#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_PARAMETERS 5
#define LENGTH_PARAMETERS 256
#define BUFFER_LENGTH 256
#define COMMANDS_LENGTH 9
#define REGISTERS 17

#include <stdint.h>
//#include <tests.h>
#include <syscall.h>

#define CMD_COUNT 18


typedef int (*CmdHandler)(int argc, char params[][LENGTH_PARAMETERS]);

typedef struct {
	const char* name;
	CmdHandler handler;
	uint8_t isBackground;
} Command;

int help(int argc, char params[MAX_PARAMETERS][LENGTH_PARAMETERS]);

int invalidOPCode(int argc, char params[][LENGTH_PARAMETERS]);

int divideByZero(int argc, char params[][LENGTH_PARAMETERS]);

int inforeg(int argc, char params[][LENGTH_PARAMETERS]);

int time(int argc, char params[][LENGTH_PARAMETERS]);

int changeFontSize(int argc, char params[][LENGTH_PARAMETERS]);

int snakes(int argc, char params[][LENGTH_PARAMETERS]);

int clearScreen(int argc, char params[][LENGTH_PARAMETERS]);

int printMem(int argc, char params[][LENGTH_PARAMETERS]);

//NEW

int kill(int argc, char params[][LENGTH_PARAMETERS]);

int loop();

int nice(int argc, char params[][LENGTH_PARAMETERS]);

int block(int argc, char argv[][LENGTH_PARAMETERS]);

int cat();

int wc();

int filter();

int ps();

int pipe();

int sem();

static Command cmds[] = {
    
	{"help", help, 0},
	{"invalidopcode", invalidOPCode, 0},
	{"inforeg", inforeg, 0},
	{"time", time, 0},
	{"changefontsize", changeFontSize, 0},
	{"snakes", snakes, 0},
	{"clear", clearScreen, 0},
	{"printmem", printMem, 0},
	{"kill",kill,0},
	{"loop",loop,0},
	{"nice",nice,0},
	{"block",block,0},
	{"cat",cat,0},
	{"wc",wc,0},
	{"filter",filter,0},
	{"ps",ps,0},
	{"pipe",pipe,0},
	{"sem",sem,0}
};

#endif