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

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"
#include "dict.h"
#include "elem.h"
#include "stack.h"
#include "cmdbuf.h"

// Deals with ."hello" print statements
void textPrint(char * text){
	assert(text != NULL);
	int i;
	int len = strlen(text) - 1;

	for(i = 2; i < len; i++){
		printf("%c", text[i]);
	}
	return;
}

int isNum(char * foo){
	int i = 0;

	if((foo == NULL) || (strlen(foo) == 0)) return 0;

	// If first is minus, scan from beginning to see if all are digits
	if((foo[0] == '-') && (strlen(foo) > 1)){
		for (i=1; i < (strlen(foo)); i++){
			if(!isdigit(foo[i])) return 0;
		}
	}else for (i=0; i < (strlen(foo)); i++){
		if(!isdigit(foo[i])) return 0;
	}
	return 1;
}

// Replaces run() from old implementation.
// TODO This should be wrriten as a proper Forth-style NEXT evaluator
void next(stack * workStack, cmdbuffer * cmdbuf, dict * vocab){
	return;
}

// TODO Write a new "parser" here (replaces stackInput() from old implementation)
// Takes command line and splits it by spaces or tabs (may contain core words, defined words, word definitions, literals, comments)
// Parses core words to function pointers, defined words to word pointers, and literals to a pointer to a "literal" coreword followed by the literal value
// Note: when defining a function, comments should be retained somehow. Keep the original text? Add a comment tag like the literal tag?
// Adds the elements to the back of the command buffer
void commandParse(char * line, cmdbuffer * cmdbuf){

	// It should initially just produce a space-separated linked list of "things".
	// Then we can modify it to produce a list of tags and pointers, Forth-style (the command buffer)
	// This will require doing the dictionary searches that used to be in wordRun() except that defiend words will also become pointers
	// To that end, we will need to modify the dictionary to contain pointer representations of user-defined words
	// Unlike the old parser, this new one should not require a space before the semocolon to terminate a word definition.
	// We should also use the cmdbuf->ready flag to indicate when the last "Statement" is completed and it is safe to evaluate the line.

	return;
}

char * prompt(int ready){
	char *line;
	if(ready){
		line = readline ("* ");
	}else{
		line = readline ("? ");
	}
	//Check for EOF.
	if (!line){
		printf("\n");
		return "BYE";
	}
	if(strcmp(line, "")) add_history(line);
	return line;
}

