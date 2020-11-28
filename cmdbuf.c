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
#include "stack.h"

cmdbuffer * newCmdBuffer(){
	cmdbuffer * new_cmdbuf = malloc(sizeof(cmdbuffer));
	new_cmdbuf->array = malloc(10*sizeof(void*));
	new_cmdbuf->capacity = 10;
	new_cmdbuf->size = 0;
	new_cmdbuf->status = 0;

	new_cmdbuf->ip = 0;

	return new_cmdbuf;
}

// Empties command buffer and resets all statuses
void cmdClear(cmdbuffer * cmdbuf) {
	cmdbuf->size = 0;
	cmdbuf->status = 0;
	return;
}

// As we add things to struct command, this needs to be expanded
void cmdAppend(cmdbuffer * cmdbuf, void * cmd) {
	if((cmdbuf->capacity) == (cmdbuf->size)) cmdGrow(cmdbuf);
	(cmdbuf->size)++;
	cmdbuf->array[cmdbuf->size-1] = cmd;
	return;
}

void cmdGrow(cmdbuffer * cmdbuf){
	cmdbuf->capacity = 2 * (cmdbuf->capacity);
	cmdbuf->array = realloc(cmdbuf->array, (cmdbuf->capacity)*sizeof(void*));
	return;
}
