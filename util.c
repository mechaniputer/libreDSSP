/*	This file is part of libreDSSP.

	Copyright 2020 Alan Beadle

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
#include "stack.h"
#include "cmdbuf.h"

#define INIT_STATEMENT_CAP (8)

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

// FIXME Doesn't need the dictionary since no lookups, all pointers
void word_next(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// TODO
	//   (IP) -> W   fetch memory pointed by IP into "W" register
	//   IP+2 -> IP  advance IP (assuming 2-byte addresses)
	//   JP (W)      jump to the address in the W register
	return;
}

// FIXME Doesn't need the dictionary since no lookups, all pointers
void word_enter(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// TODO
	//     PUSH IP     onto the "return address stack"
	//     W+2 -> IP   W still points to the Code Field, so W+2 is 
	//                 the address of the Body!  (Assuming a 2-byte
	//                 address -- other Forths may be different.)
	//     JUMP to interpreter ("NEXT")
	return;
}

// FIXME Doesn't need the dictionary since no lookups, all pointers
void word_exit(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// TODO
	//   POP IP   from the "return address stack"
	//   JUMP to interpreter
}

// Populates the command buffer. Tracks completeness of current statement.
// word_next() is called from elsewhere.
void commandParse(char * line, cmdbuffer * cmdbuf, dict * vocab){
	printf("Parsing...\n");
	int i;
	char ch;

	int statement_cap = INIT_STATEMENT_CAP;
	int statement_len = 0;
	char *statement = malloc(8*sizeof(char));
	statement[0] = '\0';

	// If there's an incomplete statement then we need to keep adding to it until it becomes complete.
	// In order to know when it's complete we will need to know what type of statement it is.
	// This parser greedily emits code for each word as encountered (including during "compile" mode)
	i=0;
	do{
		ch = line[i++];

		if (cmdbuf->status & STAT_INC_COMMENT){
			// Comments are always filtered out
			if(cmdbuf->status & STAT_INC_ESCAPE){
				cmdbuf->status &= (~STAT_INC_ESCAPE);
			}else if(ch == ']'){
				cmdbuf->status &= (~STAT_INC_COMMENT);
				printf("Comment ended at index %d\n",i-1);
			}else if(ch =='\\'){
				// Escape char, set status
				cmdbuf->status |= STAT_INC_ESCAPE;
			}
		}else if (cmdbuf->status & STAT_INC_STRING){
			if(cmdbuf->status & STAT_INC_ESCAPE){
				statement[statement_len] = ch; // FIXME check statement_cap
				statement_len++;
				cmdbuf->status &= (~STAT_INC_ESCAPE);
			}else if(ch == '"'){
				statement[statement_len] = '\0'; // FIXME check statement_cap
				cmdbuf->status &= (~STAT_INC_STRING);
				printf("String ended at index %d\n",i-1);
				cmdbuf->status &= (~STAT_INC_PRINT);
				// TODO Emit the string length and pointers and then the generic string printing word if printing

				// Now that we have detached the old statement buffer we need a new one
				statement_cap = INIT_STATEMENT_CAP;
				statement_len = 0;
				statement = malloc(8*sizeof(char));
				statement[0] = '\0';

			}else if(ch == '\\'){
				// Escape char, set status
				cmdbuf->status |= STAT_INC_ESCAPE;
			}else{
				statement[statement_len] = ch; // FIXME check statement_cap
				statement_len++;
			}
		}else if (ch == '[') { // Start of comment
			cmdbuf->status |= STAT_INC_COMMENT;
			printf("Comment starting at index %d\n",i-1);
		}else if (ch == '\"') {
			cmdbuf->status |= STAT_INC_STRING;
			printf("String starting at index %d\n",i);
			if((2<=i) && (line[i-2] == '.')){
				cmdbuf->status |= STAT_INC_PRINT;
				printf("Print flag set\n");
			}
		}else if ((ch != ' ') && (ch != '\t') && (ch != '\0')) {
			// Normal contiguous characters
			statement[statement_len] = ch; // FIXME check statement_cap
			statement_len++;

		}else if (((ch == '\0') || (ch == ' ') || (ch == '\t')) && (statement_len != 0)){ 	// Whitespace, deduplicated, including newlines
			// TODO If we just saw {IF*, BR*, DO, :, ELSE, TRAP} then we set the appropriate incomplete status flag here so we can throw an error before execution if it isn't completed.

			// TODO Check to see if last word completed a statement (branch/trap operands, VAR/VCTR assignments/declarations, semicolon) so we can clear flags

			if(statement_len != 0){
				statement[statement_len] = '\0';
				if(isNum(statement)){
					printf("LITNUM: %s\n",statement);
					// TODO literal: Create a (nameless) word that pushes it and put a pointer to that command into the buffer
				}else{
					printf("WORD: %s\n",statement);
					void * foo = (void*) coreSearch(statement, vocab);
					if(NULL == foo) foo = (void*) wordSearch(statement, vocab);
					if(NULL == foo){
						printf("Word not found\n");
					}else{
						// TODO Emit the pointers
						printf("Found\n");
					}
				}
			}
			statement_len = 0; // No need to get a new buffer since we didn't detach it
		}
	}while(ch != '\0');

	// FIXME It might seem tempting to allow words as branch conditions but words can have side effects and constants cannot. It breaks the semantics.
	// FIXME Instead, the BR word should use constants only. But we need to prevent ELSE symbol from colliding with any of the symbols.
	// FIXME Should we record the number of conditional checks before ELSE?  eg, [BR 3 C1 P1 C2 P2 C3 P3 P4]

	// Reached end of current line. Since we emit code greedily we don't need to keep any text.
	if(cmdbuf->status & STAT_INC_PRINT){
		printf("Error: Incomplete print statement\n");
		cmdbuf->status = (cmdbuf->status & (~STAT_INC_PRINT));
	}else if(cmdbuf->status & STAT_INC_STRING){
		printf("Error: Incomplete string literal\n");
		cmdbuf->status = (cmdbuf->status & (~STAT_INC_STRING));
	}else if(cmdbuf->status & STAT_INC_COMMENT){
		printf("Error: Incomplete comment\n");
		cmdbuf->status = (cmdbuf->status & (~STAT_INC_COMMENT));
	}
	free(statement);

	return;
}

char * prompt(int status){
	char *line;
	if(0 == status){
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

