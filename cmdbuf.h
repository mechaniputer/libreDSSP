/*	This file is part of libreDSSP.

	Copyright 2019 Alan Beadle

	libreDSSP is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libreDSSP is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libreDSSP.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CMDBUF_H
#define CMDBUF_H

#include "stack.h"

// Interpreter status flags
#define STAT_INC_COMMENT	0X1<<0
#define STAT_INC_STRING		0X1<<1
#define STAT_INC_PRINT		0X1<<2
#define STAT_INC_COMPILE	0X1<<3

typedef struct cmdbuffer_struct cmdbuffer;
typedef struct command_struct command;

struct cmdbuffer_struct
{
	int capacity;
	int top;
	int status; // Used by parser to note incomplete phrases
	command * array;
};

struct command_struct
{
	// What to put here?
	// Need to account for:
	// * builtin function pointers
	void (*func)(stack *, cmdbuffer *, dict *);
	// * defined words
	// * literals
	// * strings ."hello"
	// * anything that can be split by spaces
	char * text;
};

cmdbuffer * newCmdBuffer();
command * cmdTop(cmdbuffer * cmdbuf);
command * cmdPop(cmdbuffer * cmdbuf);
void cmdDrop(cmdbuffer * cmdbuf);
void cmdFree(command* to_free);
void cmdClear(cmdbuffer * cmdbuf);
void cmdPush(cmdbuffer * cmdbuf, command * cmd);
void cmdGrow(cmdbuffer * cmdbuf);
void newCommand(command * oldcmd, command ** newcmd);

#endif
