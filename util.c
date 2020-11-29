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

// Globals to define word across potentially several parser invocations
// TODO make length dynamic
char *newWordName;
char *newWordText;
int newWordTextLen;
void ***newWordCode;
int newWordCodeLen;

#define INIT_STATEMENT_CAP (8)

#define ERR_RETURN 	free(statement); cmdbuf->status = 0; return 1;

#define ERR_FORB_SYM_IN_WORD printf("Error: forbidden symbol inside word\n"); ERR_RETURN
#define ERR_FORB_SEMICOLON   printf("Error: forbidden use of semicolon\n"); ERR_RETURN
#define ERR_INC_PRINT        printf("Error: Incomplete print statement\n"); ERR_RETURN
#define ERR_INC_STRING       printf("Error: Incomplete string literal\n"); ERR_RETURN
#define ERR_NEST_COMMENT     printf("Error: comments cannot be nested\n"); ERR_RETURN
#define ERR_NEST_DEF         printf("Error: definitions cannot be nested\n"); ERR_RETURN
#define ERR_EMPTY_DEF        printf("Error: definitions must contain a name\n"); ERR_RETURN
#define ERR_PUSHLIT          printf("Error: PUSHLIT not found in core dictionary\n"); ERR_RETURN
#define ERR_DOCOLON          printf("Error: DOCOLON not found in core dictionary\n"); ERR_RETURN
#define ERR_EXIT             printf("Error: ;S not found in core dictionary\n"); ERR_RETURN
#define ERR_NO_DICT          printf("Error: No dictionary selected\n"); ERR_RETURN

#define GROW_BUFFER \
	statement_cap += INIT_STATEMENT_CAP; \
	char * newbuffer =  realloc(statement, statement_cap*sizeof(char)); \
	assert(NULL != newbuffer); \
	statement = newbuffer;

#define NEW_BUFFER \
	statement_len = 0; \
	statement_cap = INIT_STATEMENT_CAP; \
	statement =  malloc(statement_cap*sizeof(char)); \
	assert(NULL != statement); \
	statement[0] = '\0';

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

// TODO Need to allow use of undefined words via placeholder (DSSP thing)
//		"introduce a table of undefined names and some procedure to support it"
//		"table of addresses where this word is used"
//		Does this mean we need a placeholder word that just prints "undefined word" and returns to the prompt?
// TODO Need return address stack
// TODO Error handling? What if there is an error such as an undefined word being executed?
//		Currently an error clears the cmdstack which makes the loop check fail and return. This will not work for subroutines.
//		Instead we might have to use a goto or something
// TODO Might need consistent way to find dict entry name from pointer and vice-versa? If we do then maybe just add ptr search.
void word_next(){
	// Prototype (doesn't use call stack yet)
	void (*func)();
	for(cmdbuf->ip=0; cmdbuf->ip<cmdbuf->size; cmdbuf->ip++){
		func = (void*) *(cmdbuf->array[cmdbuf->ip]);
		(*func)();
	}

	// TODO The cmdbuf should contain a word that returns to the prompt so we don't need a bounded loop in this function
	// TODO Make this function as fast as possible
	//   (IP) -> W  fetch memory pointed by IP into "W" register
	//               ...W now holds address of the Code Field
	//   IP+2 -> IP advance IP, just like a program counter
	//               (assuming 2-byte addresses in the thread)
	//   (W) ->  X  fetch memory pointed by W into "X" register
	//               ...X now holds address of the machine code
	//   JP (X)     jump to the address in the X register
	return;
}

// AKA DO_COLON
// Not the same as COLON, which will be used to allocate and define a word
void word_enter(){
	printf("Hello from word_enter AKA DOCOLON\n");
	// TODO
	//   PUSH IP     onto the "return address stack"
	//   W+2 -> IP   W still points to the Code Field, so W+2 is
	//               the address of the Body!  (Assuming a 2-byte
	//               address -- other Forths may be different.)
	//   JUMP to interpreter ("NEXT")
	return;
}

// AKA ;S
// Not the same as SEMICOLON, which will finalize a new word definition
void word_exit(){
	printf("Hello from word_exit AKA ;S\n");
	// TODO
	//   POP IP   from the "return address stack"
	//   JUMP to interpreter
	return;
}

// Populates the command buffer. Tracks completeness of current statement.
// word_next() is called from elsewhere.
// return 0 if ok, 1 if error
int commandParse(char * line, dict * vocab){
	int line_ind;
	char ch, prevch;

	char * statement;
	int statement_len, statement_cap;
	NEW_BUFFER // Allocates a new buffer and sets associated counters

	// If there's an incomplete statement then we need to keep adding to it until it becomes complete.
	// In order to know when it's complete we will need to know what type of statement it is.
	// This parser eagerly emits code for each word as encountered (including during "compile" mode)
	line_ind=0;
	ch = '\0';
	prevch = '\0';
	do{
		prevch = ch;
		ch = line[line_ind++];

		// Save all text for word definition (in case we decide to save it to storage)
		// TODO make dynamic
		// TODO comments and newlines between : and name are omitted. Do we care?
		if(cmdbuf->status & STAT_INC_COMPILE){
			if(newWordText != NULL){
				if (ch == '\0'){
					newWordText[newWordTextLen++] = '\n';
				}else{
					newWordText[newWordTextLen++] = ch;
				}
			}
		}

		// FIXME Do we want to enforce whitespace after strings and comments?
		if (cmdbuf->status & STAT_INC_COMMENT){
			// Comments are always filtered out
			if(cmdbuf->status & STAT_INC_ESCAPE){
				cmdbuf->status &= (~STAT_INC_ESCAPE);
			}else if('[' == ch){
				ERR_NEST_COMMENT
			}else if(ch == ']'){
				// End of comment
				cmdbuf->status &= (~STAT_INC_COMMENT);
			}else if(ch =='\\'){
				// Escape char, set status
				cmdbuf->status |= STAT_INC_ESCAPE;
			}
		}else if (cmdbuf->status & STAT_INC_STRING){
			if(statement_len == (statement_cap - 1)){ // Resize buffer
				GROW_BUFFER
			}
			if(cmdbuf->status & STAT_INC_ESCAPE){ // Character inside string was escaped
				statement[statement_len] = ch;
				statement_len++;
				cmdbuf->status &= (~STAT_INC_ESCAPE);
			}else if(ch == '"'){ // End string
				statement[statement_len] = '\0';
				cmdbuf->status &= (~STAT_INC_STRING);
				cmdbuf->status &= (~STAT_INC_PRINT);
				// TODO Emit the string length and pointers and then the generic string printing word if printing
				// FIXME Should we try to prevent memory leaks from strings that we lose the pointer to? (with a string/var registry and periodic GC?)
				printf("String complete: %s\n",statement);
				free(statement); // TODO Temporary

				// Now that we have detached the old statement buffer we need a new one
				statement_cap = INIT_STATEMENT_CAP;
				statement_len = 0;
				statement = malloc(statement_cap*sizeof(char));
				statement[0] = '\0';

			}else if(ch == '\\'){
				// Escape char, set status
				cmdbuf->status |= STAT_INC_ESCAPE;
			}else{
				statement[statement_len] = ch;
				statement_len++;
			}
		}else if (ch == '[') { // Start of a comment
			cmdbuf->status |= STAT_INC_COMMENT;
			if(statement_len>0){
				ERR_FORB_SYM_IN_WORD
			}
		}else if (ch == '\"') { // Start of a string
			cmdbuf->status |= STAT_INC_STRING;
			if(prevch == '.'){ // String should be printed
				if(statement_len>1){
					ERR_FORB_SYM_IN_WORD
				}
				cmdbuf->status |= STAT_INC_PRINT;
			}else if(statement_len>0){
				ERR_FORB_SYM_IN_WORD
			}
			statement_len = 0; // We may have already put the "." in it if it's a print statement so we should discard that
			if(statement_cap > INIT_STATEMENT_CAP){ // Since this buffer will be detached and kept, it shouldn't be larger than needed
				free(statement);
				NEW_BUFFER
			}
		}else if ((ch != ' ') && (ch != '\t') && (ch != '\0')) {
			if((ch == ']') || (ch == '\\') || ((0 != statement_len) && (ch == '.') && (prevch != '.'))){
				ERR_FORB_SYM_IN_WORD
			}
			// Normal contiguous characters
			if(statement_len == (statement_cap - 1)){ // Resize buffer
				GROW_BUFFER
			}
			statement[statement_len] = ch;
			statement_len++;

		}else if (((ch == '\0') || (ch == ' ') || (ch == '\t')) && (statement_len != 0)){ 	// Whitespace, deduplicated, including newlines
			// TODO If we just saw {IF*, BR*, DO, :, ELSE, TRAP} then we set the appropriate incomplete status flag here so we can print a ? prompt or detect errors

			// TODO Check if last word completed a sequence (branch/trap operands, VAR/VCTR assignments/declarations) so we can clear flags
			if(statement_len != 0){
				statement[statement_len] = '\0';
				if(isNum(statement)){
					printf("LITNUM: %s\n",statement);
					// Note: Some DSSP documents say that the correct way is to adhere
					// to "one word of text, one command" and define a constant
					// procedure for each literal. We will do it the Forth way though.
					void * foo = (void*) coreSearch("PUSHLIT", vocab);
					if(NULL == foo){
						ERR_PUSHLIT
					}else{
						if(cmdbuf->status & STAT_INC_COMPILE){
							// We are in compiling mode so we emit to the current word
							newWordCode[newWordCodeLen++] = (void*) &(((coreword*)foo)->func); // Emit pointer to PUSHLIT/pushLit()
							newWordCode[newWordCodeLen++] = (void*) atol(statement); // Emit the literal
						}else{
							// We are not in compiling mode so we emit to the cmdbuf
							cmdAppend(cmdbuf, &(((coreword*)foo)->func)); // Emit pointer to PUSHLIT/pushLit()
							cmdAppend(cmdbuf, (void*)atol(statement)); // Emit the literal
						}
						statement_len = 0; // No need to get a new buffer since we didn't detach it
					}
				}else if(!strcmp(statement, ":")){ // Beginning of word declaration
					if(cmdbuf->status != 0){
						ERR_NEST_DEF
					}
					// Make sure a destination dictionary is selected
					if (vocab->grow == NULL){
						ERR_NO_DICT
					}
					printf("Entering compile mode\n");
					cmdbuf->status |= STAT_INC_COMPILE;
					// Prepare vars to build definition
					if(newWordName != NULL) free(newWordName);
					if(newWordText != NULL) free(newWordText);
					if(newWordCode != NULL) free(newWordCode);
					newWordName = NULL;
					newWordText = NULL;
					newWordCode = NULL;
					newWordTextLen = 0;
					newWordCodeLen = 0;
				}else if(!strcmp(statement, ";")){
					if(!(cmdbuf->status & STAT_INC_COMPILE)){
						ERR_FORB_SEMICOLON
					}
					if(newWordName == NULL){
						ERR_EMPTY_DEF
					}
					// Populate last code element with EXIT/;S
					void * foo = (void*) coreSearch(";S", vocab);
					if(NULL == foo){
						ERR_EXIT
					}else{
						newWordCode[newWordCodeLen++] = (void*) &(((coreword*)foo)->func); // Note presence of &
					}
					cmdbuf->status &= (~STAT_INC_COMPILE);
					printf("Definition of %s complete\n",newWordName);
					newWordText[newWordTextLen] = '\0';
					printf("%s\n",newWordText);
					for(int i=0; i<newWordCodeLen; i++){
						printf("%d: %p\n",i, newWordCode[i]);
					}
					// TODO allocate new word in appropriate dictionary, or find prior word to redefine
				}else if(cmdbuf->status & STAT_INC_COMPILE){
					if(newWordName == NULL){
						// We just found a name for the new definition
						// We can look it up to see if it is a redefinition at the very end if successful.
						// Until then we will maintain the information in separate variables
						newWordName = malloc((1+strlen(statement))*sizeof(char));
						strcpy(newWordName, statement);
						// TODO make dynamic
						newWordText = malloc(20*sizeof(char)); // allocate definition text array
						newWordCode = malloc(10*sizeof(void*)); // allocate code body
						// populate first code element with DOCOLON
						void * foo = (void*) coreSearch("DOCOLON", vocab);
						if(NULL == foo){
							ERR_DOCOLON
						}else{
							newWordCode[newWordCodeLen++] = (void*) (((coreword*)foo)->func); // Note lack of &
							// Populate start of text entry
							strcpy(newWordText, ": ");
							strcat(newWordText, newWordName);
							strcat(newWordText, " ");
							newWordTextLen = strlen(newWordText);
						}
					}else{
						// Add word to ongoing word definition
						void * foo = (void*) coreSearch(statement, vocab);
						if(NULL != foo){ // Found in core dictionary
							newWordCode[newWordCodeLen++] = (void*) &(((coreword*)foo)->func);
						}else{ // Not found in core dictionary
							foo = (void*) wordSearch(statement, vocab);
							// Emit the pointer to first element of found word code, which itself will be a pointer to the code body of DOCOLON
							if(NULL != foo){
								newWordCode[newWordCodeLen++] = (void*) (((word*)foo)->code); // Note lack of & compared to a core word
							}
						}
						if(NULL == foo){
							// TODO If undef word is used, add it to the table and emit UNDEF ptr
						}
						statement_len = 0; // No need to get a new buffer since we didn't detach it
					}
				}else{
					void * foo = (void*) coreSearch(statement, vocab);
					if(NULL != foo){ // Found in core dictionary
						cmdAppend(cmdbuf, &(((coreword*)foo)->func)); // Emit the pointer to the function pointer
					}else{ // Not found in core dictionary
						foo = (void*) wordSearch(statement, vocab);
						// Emit the pointer to the first array element, which will point to DOCOLON
						if(NULL != foo) cmdAppend(cmdbuf, (((word*)foo)->code)); // Note lack of & compared to a core word
					}
					if(NULL == foo) printf("%s not known\n",statement); // TODO abort rest of input? Use UNDEF word?
					statement_len = 0; // No need to get a new buffer since we didn't detach it
				}
			}
			statement_len = 0; // No need to get a new buffer since we didn't detach it
		}
	}while(ch != '\0');

	// FIXME It might seem tempting to allow words as branch conditions but words can have side effects and constants cannot. It breaks the semantics.
	// FIXME Instead, the BR word should use constants only. But we need to prevent ELSE symbol from colliding with any of the symbols.
	// FIXME Should we record the number of conditional checks before ELSE?  eg, [BR 3 C1 P1 C2 P2 C3 P3 P4]

	// Reached end of current line. Since we emit code eagerly we don't need to keep any text (unless we are in compile mode)
	if(cmdbuf->status & STAT_INC_PRINT){
		ERR_INC_PRINT
	}else if(cmdbuf->status & STAT_INC_STRING){
		ERR_INC_STRING
	}

	printf("status: %d\n",cmdbuf->status);
	free(statement);

	return 0;
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
		line = (char*) malloc(4*sizeof(char));
		strcpy(line, "BYE");
	}
	if(strcmp(line, "")) add_history(line);
	return line;
}

