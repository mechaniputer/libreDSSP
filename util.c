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

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "util.h"
#include "dict.h"
#include "stack.h"
#include "cmdbuf.h"

// Globals to define word across potentially several parser invocations
char *newWordName;
char *newWordText;
int newWordTextLen;
int newWordTextCap;
codeword_t **newWordCode;
int newWordCodeLen;
int newWordCodeCap;

// Global for the codeword currently being executed
codeword_t *current_codeword = NULL;

// Global return stack for nested word execution
// (defined in dssp.c, used by word_enter() and word_exit())
extern stack *returnStack;

#define INIT_STATEMENT_CAP (8)
#define INIT_WORDCODE_CAP (32)
#define INIT_WORDTEXT_CAP (32)

#define ERR_FATAL 	assert(0);

#define ERR_FORB_SYM_IN_WORD printf("Error: forbidden symbol inside word\n"); ERR_FATAL
#define ERR_FORB_SEMICOLON   printf("Error: forbidden use of semicolon\n"); ERR_FATAL
#define ERR_INC_PRINT        printf("Error: Incomplete print statement\n"); ERR_FATAL
#define ERR_INC_STRING       printf("Error: Incomplete string literal\n"); ERR_FATAL
#define ERR_NEST_COMMENT     printf("Error: comments cannot be nested\n"); ERR_FATAL
#define ERR_NEST_DEF         printf("Error: definitions cannot be nested\n"); ERR_FATAL
#define ERR_EMPTY_DEF        printf("Error: definitions must contain a name\n"); ERR_FATAL
#define ERR_NO_DICT          printf("Error: No dictionary selected\n"); ERR_FATAL
#define ERR_MISSING_CORE     printf("Error: missing word in core dictionary\n"); ERR_FATAL
#define ERR_EXIT             printf("Error: ;S not found in core dictionary\n"); ERR_FATAL

#define GROW_PARSE_BUFFER \
	statement_cap += INIT_STATEMENT_CAP; \
	char * newbuffer =  realloc(statement, statement_cap*sizeof(char)); \
	assert(NULL != newbuffer); \
	statement = newbuffer;

#define NEW_PARSE_BUFFER \
	statement_len = 0; \
	statement_cap = INIT_STATEMENT_CAP; \
	statement =  malloc(statement_cap*sizeof(char)); \
	assert(NULL != statement); \
	statement[0] = '\0';

// Makes sure we can add another cell to a word
#define CHECK_CAP_CODE                                                             \
	if (newWordCodeLen >= newWordCodeCap)                                          \
	{                                                                              \
		newWordCodeCap *= 2;                                                       \
		newWordCode = realloc(newWordCode, newWordCodeCap * sizeof(codeword_t *)); \
	}

// This takes a size param since we aren't just adding one char at a time and
// must preallocate a known size.
#define CHECK_CAP_TEXT(SZ)                                                 \
	if (newWordTextLen + (SZ) >= newWordTextCap)                           \
	{                                                                      \
		newWordTextCap += (SZ);                                            \
		newWordText = realloc(newWordText, newWordTextCap * sizeof(char)); \
	}

// Shows current cmdbuf, ip, and stack contents
void debug(){
	printf("*** DEBUG INFO ***\n");
	for(int i=0; i<= cmdbuf->size; i++){
		printf("%p: ", (void*)cmdbuf->array[i]);
		if(cmdbuf->array[i] != NULL){
			printf(" %s", cmdbuf->array[i]->name);
		}
		if(i== cmdbuf->ip){
			printf(" <IP>");
		}
		printf("\n");
	}
	showStack();
	printf("*** END ***\n");
}

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

int isNum(char * st){
	int i = 0;

	if((st == NULL) || (strlen(st) == 0)) return 0;

	// If first is minus, scan from beginning to see if all are digits
	if((st[0] == '-') && (strlen(st) > 1)){
		for (i=1; i < (strlen(st)); i++){
			if(!isdigit(st[i])) return 0;
		}
	}else for (i=0; i < (strlen(st)); i++){
		if(!isdigit(st[i])) return 0;
	}
	return 1;
}


// Executes code in cmdbuf->array until we hit a NULL sentinel codeword
// Each element is a codeword_t struct with an execution token (xt)
void word_next(){
	// Since the NULL sentinel already terminates a word, we might consider
	// removing word_exit() from word definitions and conditionally calling it
	// in this loop (based on whether we just ran a user word vs a string of
	// commands from the prompt or a file. It might be better for branch
	// prediction on modern CPUs, plus it would save some memory in user words.
	// We would also need to revise looping corewords with this change.

	cmdbuf->ip = 0;
	// TODO Consider using the size as a bound instead of a NULL sentinel (fewer loads)
	while(cmdbuf->array[cmdbuf->ip] != NULL){
		current_codeword = cmdbuf->array[cmdbuf->ip];
		(*current_codeword->xt)();
		cmdbuf->ip++;
	}
	//printf("word_next() finished looping\n");
	return;
}


// AKA DO_COLON
void word_enter(){
	//printf("word_enter() entering %s\n", current_codeword->name);

	// Push former context
	push(returnStack, (intptr_t) cmdbuf->ip);
	push(returnStack, (intptr_t) cmdbuf->array);
	//printf("word_enter() pushed return IP: %d cmdbuf->array: %p\n", (int) cmdbuf->ip, (void*)cmdbuf->array);

	// Load the new context
	cmdbuf->array = (codeword_t **) (current_codeword->data);
	cmdbuf->size = current_codeword->size;
	cmdbuf->ip = -1; // The loop in word_next() will increment this to 0 before executing the first codeword in the new array
	//printf("This word has size %d\n",cmdbuf->size);

	return; // to word_next() or a loop coreword
}

// AKA ;S
// Not the same as SEMICOLON, which will finalize a new word definition
void word_exit(){
	//printf("In word_exit()\n");

	// Restore earlier context
	cmdbuf->array = (codeword_t **) pop(returnStack);
	cmdbuf->ip = (int) pop(returnStack);
	//printf("word_exit() popped return IP: %d cmdbuf->array: %p\n", (int) cmdbuf->ip, (void*)cmdbuf->array);

	return; // to word_next() or a loop coreword
}

// Populates the command buffer. Tracks completeness of current statement.
// word_next() is called from elsewhere.
// return 0 if ok, 1 if error
int commandParse(char * line, dict * vocab){
	int line_ind;
	char ch, prevch;

	char * statement;
	int statement_len, statement_cap;
	NEW_PARSE_BUFFER

	// If there's an incomplete statement then we need to keep adding to it until it becomes complete.
	// In order to know when it's complete we will need to know what type of statement it is.
	// This parser eagerly emits code for each word as encountered (including during "compile" mode)
	line_ind=0;
	ch = '\0';
	prevch = '\0';

	// Each iteration processes one character and updates the command buffer
	// and parser status accordingly. We keep track of the previous character
	// to detect forbidden symbol sequences.
	do{
		prevch = ch;
		ch = line[line_ind++];

		// Save all text for word definition
		// TODO comments and newlines between : and name are omitted. Do we care?
		if(cmdbuf->status & STAT_INC_COMPILE){
			if(newWordText != NULL){
				CHECK_CAP_TEXT(1)
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
				GROW_PARSE_BUFFER
			}
			if(cmdbuf->status & STAT_INC_ESCAPE){ // Character inside string was escaped
				statement[statement_len] = ch;
				statement_len++;
				cmdbuf->status &= (~STAT_INC_ESCAPE);
			}else if(ch == '"'){ // End string
				statement[statement_len] = '\0';
				cmdbuf->status &= (~STAT_INC_STRING);
				// FIXME Should we try to prevent memory leaks from strings that we lose the pointer to? (with a string/var registry and periodic GC?)
				printf("String complete: %s\n",statement);

				if(cmdbuf->status & STAT_INC_PRINT){
					printf("Emitting print codewords for string\n");
					cmdbuf->status &= (~STAT_INC_PRINT);
					// Emit a codeword that pushes the string pointer
					codeword_t * push_ptr = newLiteral((intptr_t) statement);
					codeword_t * push_len = newLiteral((intptr_t) strlen(statement));
					codeword_t * print_st = coreSearch("TOS", vocab);
					if(print_st == NULL){
						ERR_MISSING_CORE
					}
					if(cmdbuf->status & STAT_INC_COMPILE){
						// Add codewords to word definition
						CHECK_CAP_CODE
						newWordCode[newWordCodeLen++] = push_ptr;
						CHECK_CAP_CODE
						newWordCode[newWordCodeLen++] = push_len;
						CHECK_CAP_CODE
						newWordCode[newWordCodeLen++] = print_st;
					}else{
						// Add codewords to command buffer
						cmdAppend(cmdbuf, push_ptr);
						cmdAppend(cmdbuf, push_len);
						cmdAppend(cmdbuf, print_st);
					}
				}else{
					// TODO: Handle the case where the string is not marked for printing
					free(statement); // TODO Temporary
				}
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
				// Remove the '.' from the statement buffer since it is not part of the string
				statement_len--;
				statement[statement_len] = '\0';
			}else if(statement_len>0){
				ERR_FORB_SYM_IN_WORD
			}
			if(statement_cap > INIT_STATEMENT_CAP){ // Since this buffer will be detached and kept, it shouldn't be larger than needed
				free(statement);
				NEW_PARSE_BUFFER
			}
		}else if ((ch != ' ') && (ch != '\t') && (ch != '\0')) {
			if((ch == ']') || (ch == '\\') || ((0 != statement_len) && (ch == '.') && (prevch != '.'))){
				ERR_FORB_SYM_IN_WORD
			}
			// Normal contiguous characters
			if(statement_len == (statement_cap - 1)){ // Resize buffer
				GROW_PARSE_BUFFER
			}
			statement[statement_len] = ch;
			statement_len++;

		}else if (((ch == '\0') || (ch == ' ') || (ch == '\t')) && (statement_len != 0)){ 	// Whitespace, deduplicated, including newlines
			statement[statement_len] = '\0';
			if(isNum(statement)){
				codeword_t *lit_cw = newLiteral(atol(statement));

				if(cmdbuf->status & STAT_INC_COMPILE){
					// Add literal codeword to word definition
					CHECK_CAP_CODE
					newWordCode[newWordCodeLen++] = lit_cw;
				}else{
					// Add literal codeword to command buffer
					cmdAppend(cmdbuf, lit_cw);
				}
			}else if(!strcmp(statement, ":")){ // Beginning of word declaration
				if(cmdbuf->status != 0){
					printf("Error Status is %d\n",cmdbuf->status);
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
				newWordCode = malloc(INIT_WORDCODE_CAP * sizeof(codeword_t*));
				newWordCodeCap = INIT_WORDCODE_CAP;
				newWordName = NULL;
				newWordText = malloc(INIT_WORDTEXT_CAP * sizeof(char)); // allocate definition text array
				newWordTextCap = INIT_WORDTEXT_CAP;
				newWordTextLen = 0;
				newWordCodeLen = 0;
			}else if(!strcmp(statement, ";")){
				if(!(cmdbuf->status & STAT_INC_COMPILE)){
					ERR_FORB_SEMICOLON
				}
				if(newWordName == NULL){
					ERR_EMPTY_DEF
				}
				// Populate last code element with word_exit() AKA ;S
				codeword_t * dict_entry = coreSearch(";S", vocab);
				if(NULL == dict_entry){
					ERR_EXIT
				}else{
					CHECK_CAP_CODE
					newWordCode[newWordCodeLen++] = dict_entry;
					CHECK_CAP_CODE
					newWordCode[newWordCodeLen] = NULL;  // Restore NULL sentinel at end of code array now that we know the final length
				}
				cmdbuf->status &= (~STAT_INC_COMPILE);
				printf("Definition of %s complete\n",newWordName);
				CHECK_CAP_TEXT(1)
				// Remove trailing newlines in definition text
				while(newWordTextLen > 0 && newWordText[newWordTextLen - 1] == '\n'){
					newWordTextLen--;
				}
				newWordText[newWordTextLen] = '\0';
				printf("%s\n",newWordText);

				// Allocate new word in appropriate dictionary, or find prior word to redefine
				dict_entry = wordDefine(newWordName, vocab);
				// Populate the dictionary entry
				dict_entry->data = (intptr_t) newWordCode;  // Set code array pointer
				dict_entry->size = newWordCodeLen;
				dict_entry->text = newWordText;

				// Detach
				newWordCode = NULL;
				newWordText = NULL;
			}else if(!strcmp(statement, "GROW") || !strcmp(statement, "USE") || !strcmp(statement, "SHUT") || !strcmp(statement, "ONLY")){
				if(cmdbuf->status & STAT_INC_COMPILE){
					ERR_FORB_SYM_IN_WORD
				}
				cmdbuf->status |= STAT_INC_DICT_OP;
				// Get the codeword for this operation
				codeword_t * dict_entry = coreSearch(statement, vocab);
				// Emit codeword to command buffer. We will update the data field later once we have the dictionary name.
				cmdAppend(cmdbuf, dict_entry);
			}else if(!strcmp(statement, "DO")){
				cmdbuf->status |= STAT_INC_DO_LOOP;
				codeword_t *dict_entry = coreSearch("DO", vocab);
				if(dict_entry == NULL){
					ERR_MISSING_CORE
				}
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					CHECK_CAP_CODE
					newWordCode[newWordCodeLen++] = dict_entry;
				}else{
					cmdAppend(cmdbuf, dict_entry);
				}
			}else if(!strcmp(statement, "BR")){
				cmdbuf->status |= STAT_BR_NO_ELSE;
				codeword_t *dict_entry = coreSearch("BR", vocab);
				if(dict_entry == NULL){
					ERR_MISSING_CORE
				}
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					CHECK_CAP_CODE
					newWordCode[newWordCodeLen++] = dict_entry;
				}else{
					cmdAppend(cmdbuf, dict_entry);
				}
			}else if((cmdbuf->status & STAT_BR_NO_ELSE) && !strcmp(statement, "ELSE")){
				printf("Found an ELSE\n");
				cmdbuf->status &= (~STAT_BR_NO_ELSE);
				cmdbuf->status |= STAT_BR_ELSE;
				codeword_t *dict_entry = coreSearch("ELSE", vocab);
				if(dict_entry == NULL){
					ERR_MISSING_CORE
				}
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					CHECK_CAP_CODE
					newWordCode[newWordCodeLen++] = dict_entry;
				}else{
					cmdAppend(cmdbuf, dict_entry);
				}
			}else if(cmdbuf->status & STAT_INC_DICT_OP){
				cmdbuf->status &= (~STAT_INC_DICT_OP);
				// The codeword that we need to update is the last one we emitted.
				codeword_t * last_cw = cmdbuf->array[cmdbuf->size - 1];
				// We won't do the lookup here. Instead we attach the dictionary name as text. The coreword will handle the rest.
				last_cw->text = malloc((1+strlen(statement))*sizeof(char));
				strcpy(last_cw->text, statement);
			}else if(cmdbuf->status & STAT_INC_COMPILE){
				if(newWordName == NULL){
					// We just found a name for the new definition
					// We can look it up to see if it is a redefinition at the very end if successful.
					// Until then we will maintain the information in separate variables
					newWordName = malloc((1+strlen(statement))*sizeof(char));
					strcpy(newWordName, statement);
					// newWordCode already allocated in compile mode setup
					// Populate start of text entry
					CHECK_CAP_TEXT(strlen(newWordName)+3)
					strcpy(newWordText, ": ");
					strcat(newWordText, newWordName);
					strcat(newWordText, " ");
					newWordTextLen = strlen(newWordText);
				}else{
					// Add word to ongoing word definition
					codeword_t *dict_entry = coreSearch(statement, vocab);
					if(NULL != dict_entry){ // Found in core dictionary
						CHECK_CAP_CODE
						newWordCode[newWordCodeLen++] = dict_entry;
						// If this finishes a BR-ELSE, reset that status
						if(cmdbuf->status & STAT_BR_ELSE){
							printf("Found ELSE condition (coreword) inside compiled word\n");
							cmdbuf->status &= (~STAT_BR_ELSE);
						}else if(cmdbuf->status & STAT_INC_DO_LOOP){
							// If this finishes a DO, reset that status
							printf("Found core word %s inside DO loop\n", statement);
							cmdbuf->status &= (~STAT_INC_DO_LOOP);
							// Emit LOOP codeword after the single-word loop body
							codeword_t *loop_cw = coreSearch("LOOP", vocab);
							if(loop_cw == NULL){
								ERR_MISSING_CORE
							}
							CHECK_CAP_CODE
							newWordCode[newWordCodeLen++] = loop_cw;
						}
					}else{ // Not found in core dictionary
						dict_entry = wordSearch(statement, vocab);
						// Emit the pointer to first element of found word code
						if(NULL != dict_entry){
							CHECK_CAP_CODE
							newWordCode[newWordCodeLen++] = dict_entry;
							// If this finishes a BR-ELSE, reset that status
							if(cmdbuf->status & STAT_BR_ELSE){
								printf("Found ELSE condition (user word) inside compiled word\n");
								cmdbuf->status &= (~STAT_BR_ELSE);
							}else if(cmdbuf->status & STAT_INC_DO_LOOP){
								// If this finishes a DO, reset that status
								printf("Found user word %s inside DO loop\n", statement);
								cmdbuf->status &= (~STAT_INC_DO_LOOP);
								// Emit LOOP codeword after the single-word loop body
								codeword_t *loop_cw = coreSearch("LOOP", vocab);
								if(loop_cw == NULL){
									ERR_MISSING_CORE
								}
								CHECK_CAP_CODE
								newWordCode[newWordCodeLen++] = loop_cw;
							}

						}else{
							// TODO If undef word is used, add it to the table and emit UNDEF ptr
							printf("Unknown word %s found in word definition\n",statement);
						}
					}
				}
			}else{
				// Normal execution mode - emit word to cmdbuf
				codeword_t *dict_entry = coreSearch(statement, vocab);
				if(NULL != dict_entry){
					cmdAppend(cmdbuf, dict_entry);
					if(cmdbuf->status & STAT_BR_ELSE){
						cmdbuf->status &= (~STAT_BR_ELSE);
					}else if(cmdbuf->status & STAT_INC_DO_LOOP){
						// If this finishes a DO, reset that status
						printf("Found core word %s inside DO loop\n", statement);
						cmdbuf->status &= (~STAT_INC_DO_LOOP);
						// Emit LOOP codeword after the single-word loop body
						codeword_t *loop_cw = coreSearch("LOOP", vocab);
						if(loop_cw == NULL){
							ERR_MISSING_CORE
						}
						cmdAppend(cmdbuf, loop_cw);
					}
				}else{
					dict_entry = wordSearch(statement, vocab);
					if(NULL != dict_entry){
						cmdAppend(cmdbuf, dict_entry);
						if(cmdbuf->status & STAT_BR_ELSE){
							cmdbuf->status &= (~STAT_BR_ELSE);
						}else if(cmdbuf->status & STAT_INC_DO_LOOP){
							// If this finishes a DO, reset that status
							printf("Found user word %s inside DO loop\n", statement);
							cmdbuf->status &= (~STAT_INC_DO_LOOP);
							// Emit LOOP codeword after the single-word loop body
							codeword_t *loop_cw = coreSearch("LOOP", vocab);
							if(loop_cw == NULL){
								ERR_MISSING_CORE
							}
							cmdAppend(cmdbuf, loop_cw);
						}
					}
				}
				if(NULL == dict_entry) printf("%s not known\n",statement); // TODO abort rest of input? Use UNDEF word?
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

	//printf("status: %d\n",cmdbuf->status);
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

