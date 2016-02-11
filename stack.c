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
#include <malloc.h>
#include "stack.h"

stack * newStack() {
	stack * stack = malloc(sizeof(stack));
	stack->array = malloc(10*sizeof(int)); // TODO Make dynamic
	stack->capacity = 10;
	stack->top = -1; // -1 indicates empty stack
	return stack;
}

// If stack is empty, do not use!
int top(stack * stack) {
	return (stack->array[stack->top]);
}

// If stack is empty, do not use!
int pop(stack * stack) {
	return (stack->array[(stack->top)--]);
}

void push(stack * stack, int value) {
	(stack->top)++;
	stack->array[stack->top] = value;
	if((stack->capacity) == ((stack->top)+1)) grow(stack);
	return;
}

void grow(stack * stack){
	stack->capacity = 2 * (stack->capacity);
	stack->array = realloc(stack->array, (stack->capacity)*sizeof(int));
}

cmdstack * newCmdStack(){
	cmdstack * cmdstack = malloc(sizeof(cmdstack));
	cmdstack->array = malloc(10*sizeof(char *)); // TODO Make dynamic
	cmdstack->capacity = 10;
	cmdstack->top = -1; // -1 indicates empty stack
	return cmdstack;
}

// If stack is empty, do not use!
char * cmdTop(cmdstack * cmdstack) {
	return (cmdstack->array[cmdstack->top]);
}

// If stack is empty, do not use!
char * cmdPop(cmdstack * cmdstack) {
	return (cmdstack->array[(cmdstack->top)--]);
}
void cmdPush(cmdstack * cmdstack, char * cmd) {
	(cmdstack->top)++;
	cmdstack->array[cmdstack->top] = cmd;
	if((cmdstack->capacity) == ((cmdstack->top)+1)) cmdGrow(cmdstack);
	return;
}

void cmdGrow(cmdstack * cmdstack){
	cmdstack->capacity = 2 * (cmdstack->capacity);
	cmdstack->array = realloc(cmdstack->array, (cmdstack->capacity)*sizeof(char *));
}
