/*	This file is part of libreDSSP.

	Copyright 2026 Alan Beadle

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
#include "corewords.h"

cmdbuffer * newCmdBuffer(){
	cmdbuffer * new_cmdbuf = malloc(sizeof(cmdbuffer));
	new_cmdbuf->array = malloc(10*sizeof(codeword_t*));
	new_cmdbuf->capacity = 10;
	new_cmdbuf->size = 0;
	new_cmdbuf->status = 0;
	new_cmdbuf->ip = 0;

	// NULL sentinel to end codeword array
	new_cmdbuf->array[0] = NULL;

	return new_cmdbuf;
}

// Empties command buffer and resets all statuses
// FIXME changing the contents of the cmdbuf is incorrect because it may be a reference to a user word's definition.
//       In addition, if we are reading a code file, we should halt and go to the prompt on an error.
void cmdClear(cmdbuffer * cmdbuf) {
	cmdbuf->size = 0;
	cmdbuf->status = 0;
	// NULL sentinel at index 0 will cleanly do nothing
	if(cmdbuf->capacity > 0) cmdbuf->array[0] = NULL;
	return;
}

// As we add things to struct command, this needs to be expanded
void cmdAppend(cmdbuffer * cmdbuf, codeword_t * cw) {
	if((cmdbuf->capacity) == (cmdbuf->size+1)) cmdGrow(cmdbuf);
	cmdbuf->array[cmdbuf->size++] = cw;
	cmdbuf->array[cmdbuf->size] = NULL;  // Maintain NULL sentinel
	return;
}

void cmdGrow(cmdbuffer * cmdbuf){
	cmdbuf->capacity = 2 * (cmdbuf->capacity);
	cmdbuf->array = realloc(cmdbuf->array, (cmdbuf->capacity)*sizeof(codeword_t*));
	return;
}

codeword_t * newLiteral(intptr_t value) {
	codeword_t *cw = malloc(sizeof(codeword_t));
	cw->xt = pushLiteral;
	cw->data = value;
	cw->name = "(literal)";
	cw->text = NULL;
	cw->next = NULL;
	cw->user = 0; // Not user-defined word
	return cw;
}

codeword_t * newVarDecl(char * name){
	codeword_t *cw = malloc(sizeof(codeword_t));
	cw->xt = declareVar;
	cw->data = (intptr_t) malloc(1+strlen(name));
	strcpy((char *)cw->data, name);
	cw->name = "(var decl)";
	cw->text = NULL;
	cw->user = 0; // Not user-defined word
	return cw;
}

codeword_t * newVarAsgn(variable * dest) {
	codeword_t *cw = malloc(sizeof(codeword_t));
	cw->xt = assignVar;
	cw->data = (intptr_t) dest;
	cw->name = "(var asgn)";
	cw->text = NULL;
	cw->user = 0; // Not user-defined word
	return cw;
}

codeword_t * newVarPush(variable * var){
	codeword_t *cw = malloc(sizeof(codeword_t));
	cw->xt = pushVar;
	cw->data = (intptr_t) var;
	cw->name = "(var push)";
	cw->text = NULL;
	cw->user = 0; // Not user-defined word
	return cw;
}