/*	This file is part of libreDSSP.

	Copyright 2016 Alan Beadle

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

typedef struct stack stack;
struct stack
{
	int capacity;
	int top;
	int * array;
};

typedef struct cmdstack cmdstack;
struct cmdstack
{
	int capacity;
	int top;
	// Dynamic array of pointers to strings
	char ** array;
};

stack * newStack();
int top(stack * stack);
int pop(stack * stack);
void push(stack * stack, int value);
void grow(stack * stack);

cmdstack * newCmdStack();
char * cmdTop(cmdstack * cmdstack);
char * cmdPop(cmdstack * cmdstack);
void cmdPush(cmdstack * cmdstack, char * cmd);
void cmdGrow(cmdstack * cmdstack);

#endif
