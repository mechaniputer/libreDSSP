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
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "stack.h"
#include "elem.h"

stack * newStack() {
	stack * new_stack = malloc(sizeof(stack));
	new_stack->array = malloc(10*sizeof(int));
	new_stack->capacity = 10;
	new_stack->top = -1; // -1 indicates empty stack
	return new_stack;
}

// If stack is empty, do not use!
int top(stack * workStack) {
	assert(workStack->top > -1);
	return (workStack->array[workStack->top]);
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
	cmdstack * new_cmdstack = malloc(sizeof(cmdstack));
	new_cmdstack->array = malloc(10*sizeof(command));
	new_cmdstack->capacity = 10;
	new_cmdstack->top = -1; // -1 indicates empty stack
	new_cmdstack->unfinished_comment = 0;
	new_cmdstack->unfinished_func = 0;
	new_cmdstack->incomplete_tail = NULL;
	return new_cmdstack;
}

// If stack is empty, do not use!
command * cmdTop(cmdstack * cmdstack) {
	assert(cmdstack->top > -1);
	return &(cmdstack->array[cmdstack->top]);
}

// If stack is empty, do not use!
void cmdDrop(cmdstack * cmdstack) {
	assert(cmdstack->top > -1);
	cmdFree(&(cmdstack->array[(cmdstack->top)--]));
}

// If stack is empty, do not use!
command * cmdPop(cmdstack * cmdstack) {
	assert(cmdstack->top > -1);
	return &(cmdstack->array[(cmdstack->top)--]);
}

// A place to put the stuff you need to do when discarding a command
void cmdFree(command * to_free){
	if(to_free->text != NULL) free(to_free->text);
}

// Dumps all pending commands and returns
void cmdClear(cmdstack * cmdstack) {
	cmdstack->top = -1;
	cmdstack->unfinished_comment = 0;
	cmdstack->unfinished_func = 0;
	cmdstack->incomplete_tail = NULL;
	return;
}

// As we add things to struct command, this needs to be expanded
// FIXME What is even going on here? Why don't we just store a reference to the struct?
// FIXME This should be fixed when the command stack is rewritten as a queue.
void cmdPush(cmdstack * cmdstack, command * cmd) {
	(cmdstack->top)++;
	cmdstack->array[cmdstack->top].text = malloc((strlen(cmd->text)+1) * sizeof(char));
	strcpy(cmdstack->array[cmdstack->top].text, cmd->text);
	cmdstack->array[cmdstack->top].func = cmd->func;
	if((cmdstack->capacity) == ((cmdstack->top)+1)) cmdGrow(cmdstack);
	return;
}

void cmdGrow(cmdstack * cmdstack){
	cmdstack->capacity = 2 * (cmdstack->capacity);
	cmdstack->array = realloc(cmdstack->array, (cmdstack->capacity)*sizeof(command));
	return;
}

// Takes a pointer to a command pointer and fills it in with a command
void newCommand(command * oldcmd, command ** newcmd){
	*newcmd = malloc(sizeof(struct command));
	(*newcmd)->text = malloc((strlen(oldcmd->text)+1) * sizeof(char));
	strcpy((*newcmd)->text, oldcmd->text);
	(*newcmd)->func = oldcmd->func;
	return;
}
