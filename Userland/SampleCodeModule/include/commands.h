#pragma once
#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_PARAMETERS 5
#define LENGTH_PARAMETERS 256
#define BUFFER_LENGTH 256
#define COMMANDS_LENGTH 9
#define REGISTERS 21

#include <stdint.h>
#include <tests.h>
#include <syscall.h>

#define CMD_COUNT 21
#define SEM_READ 3

typedef int (*CmdHandler)(int argc, const char* argv[]);

typedef struct {
	const char* name;
	CmdHandler handler;
	uint8_t isBackground;
} Command;

int help(int argc, const char* argv[]);

int invalidOPCode(int argc, const char* argv[]);

int divideByZero(int argc, const char* argv[]);

int inforeg(int argc, const char* argv[]);

int time(int argc, const char* argv[]);

int changeFontSize(int argc, const char* argv[]);

int snakes(int argc, const char* argv[]);

int clearScreen(int argc, const char* argv[]);

int printMem(int argc, const char* argv[]);

//NEW

int kill(int argc, const char* argv[]);

int loop();

int nice(int argc, const char* argv[]);

int block(int argc, const char* argv[]);

int cat();

int wc();

int filter();

int ps();

void philosopherManager();

int printcmd();

static Command cmds[] = {
    
	{"help", help, 0},
	{"divbyzero", divideByZero, 0},
	{"invalidopcode", invalidOPCode, 0},
	{"inforeg", inforeg, 0},
	{"time", time, 0},
	{"changefontsize", changeFontSize, 0},
	{"clear", clearScreen, 0},
	{"mem", printMem, 0},
	{"kill",kill,0},
	{"loop",loop,0},
	{"nice",nice,0},
	{"block",block,0},
	{"cat",cat,0},
	{"wc",wc,0},
	{"filter",filter,0},
	{"ps",ps,0},
	{"phylo",(CmdHandler)philosopherManager,0},
	{"test_mm",(CmdHandler)test_mm,0},
	{"test_prio",(CmdHandler)test_prio,0},
	{"test_processes",(CmdHandler)test_processes,0},
	{"test_sync",(CmdHandler)test_sync,0}
};

#endif