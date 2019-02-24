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
#include "cmdbuf.h"
#include "elem.h"
#include "stack.h"

cmdbuffer * newCmdBuffer(){
	cmdbuffer * new_cmdbuf = malloc(sizeof(cmdbuffer));
	new_cmdbuf->array = malloc(10*sizeof(command));
	new_cmdbuf->capacity = 10;
	new_cmdbuf->top = -1; // -1 indicates empty buffer
	new_cmdbuf->ready = 1;
	return new_cmdbuf;
}

// If buffer is empty, do not use!
command * cmdTop(cmdbuffer * cmdbuf) {
	assert(cmdbuf->top > -1);
	return &(cmdbuf->array[cmdbuf->top]);
}

// If buffer is empty, do not use!
void cmdDrop(cmdbuffer * cmdbuf) {
	assert(cmdbuf->top > -1);
	cmdFree(&(cmdbuf->array[(cmdbuf->top)--]));
}

// If buffer is empty, do not use!
command * cmdPop(cmdbuffer * cmdbuf) {
	assert(cmdbuf->top > -1);
	return &(cmdbuf->array[(cmdbuf->top)--]);
}

// A place to put the stuff you need to do when discarding a command
void cmdFree(command * to_free){
	if(to_free->text != NULL) free(to_free->text);
}

// Dumps all pending commands and returns
void cmdClear(cmdbuffer * cmdbuf) {
	cmdbuf->top = -1;
	cmdbuf->ready = 1;
	return;
}

// As we add things to struct command, this needs to be expanded
// FIXME What is even going on here? Why don't we just store a reference to the struct?
// FIXME This should be fixed when the command buffer is rewritten as a queue.
void cmdPush(cmdbuffer * cmdbuf, command * cmd) {
	(cmdbuf->top)++;
	cmdbuf->array[cmdbuf->top].text = malloc((strlen(cmd->text)+1) * sizeof(char));
	strcpy(cmdbuf->array[cmdbuf->top].text, cmd->text);
	cmdbuf->array[cmdbuf->top].func = cmd->func;
	if((cmdbuf->capacity) == ((cmdbuf->top)+1)) cmdGrow(cmdbuf);
	return;
}

void cmdGrow(cmdbuffer * cmdbuf){
	cmdbuf->capacity = 2 * (cmdbuf->capacity);
	cmdbuf->array = realloc(cmdbuf->array, (cmdbuf->capacity)*sizeof(command));
	return;
}

// Takes a pointer to a command pointer and fills it in with a command
void newCommand(command * oldcmd, command ** newcmd){
	*newcmd = malloc(sizeof(command));
	(*newcmd)->text = malloc((strlen(oldcmd->text)+1) * sizeof(char));
	strcpy((*newcmd)->text, oldcmd->text);
	(*newcmd)->func = oldcmd->func;
	return;
}
