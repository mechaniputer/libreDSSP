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
#include <assert.h>
#include "stack.h"

stack * newStack() {
	stack * stack = malloc(sizeof(stack));
	stack->array = malloc(10*sizeof(int));
	stack->capacity = 10;
	stack->top = -1; // -1 indicates empty stack
	return stack;
}

// If stack is empty, do not use!
int top(stack * stack) {
	assert(stack->top > -1);
	return (stack->array[stack->top]);
}

// If stack is empty, do not use!
int pop(stack * stack) {
	assert(stack->top > -1);
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
	cmdstack->array = malloc(10*sizeof(command));
	cmdstack->capacity = 10;
	cmdstack->top = -1; // -1 indicates empty stack
	return cmdstack;
}

// If stack is empty, do not use!
char * cmdTop(cmdstack * cmdstack) {
	assert(cmdstack->top > -1);
	return (cmdstack->array[cmdstack->top].text);
}

// If stack is empty, do not use!
char * cmdPop(cmdstack * cmdstack) {
	assert(cmdstack->top > -1);
	return (cmdstack->array[(cmdstack->top)--].text);
}
void cmdPush(cmdstack * cmdstack, char * cmd) {
	(cmdstack->top)++;
	cmdstack->array[cmdstack->top].text = cmd;
	cmdstack->array[cmdstack->top].func = NULL; // TODO: Do something better than blindly assign NULL
	if((cmdstack->capacity) == ((cmdstack->top)+1)) cmdGrow(cmdstack);
	return;
}

void cmdGrow(cmdstack * cmdstack){
	cmdstack->capacity = 2 * (cmdstack->capacity);
	cmdstack->array = realloc(cmdstack->array, (cmdstack->capacity)*sizeof(command));
}
