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

#ifndef STACK_H
#define STACK_H

#include "elem.h"

typedef struct variable variable;
typedef struct word word;
typedef struct coreword coreword;
typedef struct subdict subdict;
typedef struct dict dict;

typedef struct stack stack;
typedef struct cmdstack cmdstack;
typedef struct command command;

struct stack
{
	int capacity;
	int top;
	int * array;
};

struct cmdstack
{
	int capacity;
	int top;
	int unfinished_comment;
	int unfinished_func;
	elem * incomplete_tail;
	command * array;
};

struct command
{
	// What to put here?
	// Need to account for:
	// * builtin function pointers
	void (*func)(stack *, cmdstack *, dict *);
	// * defined words
	// * literals
	// * strings ."hello"
	// * anything that can be split by spaces
	char * text;
};


stack * newStack();
int top(stack * stack);
int pop(stack * stack);
void push(stack * stack, int value);
void grow(stack * stack);

cmdstack * newCmdStack();
command * cmdTop(cmdstack * cmdstack);
command * cmdPop(cmdstack * cmdstack);
void cmdClear(cmdstack * cmdstack);
void cmdPush(cmdstack * cmdstack, command * cmd);
void cmdGrow(cmdstack * cmdstack);
void newCommand(command * oldcmd, command ** newcmd);

#endif
