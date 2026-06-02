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
#include <editline/readline.h>

#include "util.h"
#include "dict.h"
#include "stack.h"
#include "cmdbuf.h"

// Globals to define word across potentially several parser invocations
char *newWordNameOld;
char *newWordTextOld;
int newWordTextLenOld;
int newWordTextCapOld;
codeword_t **newWordCodeOld;
int newWordCodeLenOld;
int newWordCodeCapOld;
codeword_t * newWordDictEntryOld;

// Global for the codeword currently being executed
codeword_t *current_codeword = NULL;

// Global return stack for nested word execution
// (defined in dssp.c, used by word_enter() and word_exit())
extern stack *returnStack;
extern dict * vocab;

#define INIT_STATEMENT_CAP (8)
#define INIT_WORDCODE_CAP (16)
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
#define ERR_NAME_CONFLICT    printf("Error: Name conflict between var and word\n"); ERR_FATAL

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
	if (newWordCodeLenOld >= newWordCodeCapOld)                                          \
	{                                                                              \
		newWordCodeCapOld *= 2;                                                       \
		newWordCodeOld = realloc(newWordCodeOld, newWordCodeCapOld * sizeof(codeword_t *)); \
	}

// This takes a size param since we aren't just adding one char at a time and
// must preallocate a known size.
#define CHECK_CAP_TEXT(SZ)                                                 \
	if (newWordTextLenOld + (SZ) >= newWordTextCapOld)                           \
	{                                                                      \
		newWordTextCapOld += (SZ);                                            \
		newWordTextOld = realloc(newWordTextOld, newWordTextCapOld * sizeof(char)); \
	}

// Shows current ip, cmdbuf, and stack contents
void debug(){
	printf("*** DEBUG INFO ***\n");
	printf("IP = %d\n",cmdbuf->ip);
	print_codewords(cmdbuf->array);
	printf("Data Stack: ");
	showStack();
	printf("*** END ***\n");
}

// Takes a pointer to any NULL-terminated array of codewords
// Displays the contents/meaning of each one in sequence
// Essentially a DSSP decompiler
void print_codewords(codeword_t ** array){
	// Note: This hack is necessary to avoid segfaulting on codewords containing literal values
	int loop_end_index = -1;

	int i=0;
	while((array[i] != NULL) || (i < loop_end_index)){
		// Either we are looking at a BR literal or we are not at the NULL sentinel yet

		// The condition above determines whether we are looking at a literal value
		if((i <= loop_end_index) && ((loop_end_index-i) % 3 == 0)){
			printf("%d) %p LIT %ld\n", i, (void*)&array[i], (intptr_t) cmdbuf->array[i]);
			i++;
		}

		// If we were pointing at a literal before, we incremented i and should see a named word.
		printf("%d) %p CMD %s\n", i, (void*)&array[i], array[i]->name);
		if(!strcmp("PUSHLIT", array[i]->name)){
			// PUSHLIT is followed by a literal that we don't want to dereference
			i++;
			printf("%d) %p: Literal %ld\n", i, (void*) &array[i], (intptr_t) array[i]);
		}else if(!strcmp("VAR", array[i]->name) || !strcmp("GROW", array[i]->name)){
			// VAR is followed by a char * containing a variable name
			// GROW is followed by a char * containing a dict name
			i++;
			printf("%d) %p: STR %s\n", i, (void*) &array[i], (char *) array[i]);
		}else if(!strcmp("PUSHVAR", array[i]->name) || !strcmp("!", array[i]->name)){
			// PUSHVAR/! are followed by a pointer to a variable struct
			i++;
			printf("%d) %p: VAR %s\n", i, (void*) &array[i], ((variable_t *) array[i])->name);
		}else if(!strcmp("SHUT", array[i]->name) || !strcmp("USE", array[i]->name)){
			// SHUT/USE are followed by a pointer to a variable struct
			i++;
			printf("%d) %p: DICT %s\n", i, (void*) &array[i], ((subdict *) array[i])->name);
		}else if(!strcmp(array[i]->name, "BR")){
			// Example code:    BR 0 FOO 1 BAR 2 BAZ ELSE ERG
			// Compiled result: BR 0 FOO SKP2 1 BAR SKP2 2 BAZ SKP1 ERG
			for(loop_end_index = i+3; ; loop_end_index+=3){
				printf("%ld\n",(intptr_t) array[loop_end_index]);
				if(array[loop_end_index]->xt != skip2) break;
			}
			// loop_end_index now points to ERG in the example above (position 9)
			// We want it to point to the last literal (position 7):
			loop_end_index -= 2;
			printf("Loop end index is %d\n",loop_end_index);
		}
		i++;
	}
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
		//debug();
		current_codeword = cmdbuf->array[cmdbuf->ip];
		(*current_codeword->xt)();
		cmdbuf->ip++;
	}
	printf("word_next() finished looping\n");
	return;
}


// AKA DO_COLON
void word_enter(){
	printf("word_enter() entering %s\n", current_codeword->name);

	// Push former context
	push(returnStack, (intptr_t) cmdbuf->ip);
	push(returnStack, (intptr_t) cmdbuf->array);
	push(returnStack, (intptr_t) cmdbuf->size);
	//printf("word_enter() pushed return IP: %d cmdbuf->array: %p\n", (int) cmdbuf->ip, (void*)cmdbuf->array);
	//printf("Return stack height is %d\n",returnStack->top);

	// Load the new context
	cmdbuf->array = (codeword_t **) (current_codeword->data);
	cmdbuf->size = current_codeword->size;
	cmdbuf->ip = -1; // The loop in word_next() will increment this to 0 before executing the first codeword in the new array
	//printf("This word has size %d\n",cmdbuf->size);
	//debug();

	return; // to word_next() or a loop coreword
}

// AKA ;S
// Not the same as SEMICOLON, which will finalize a new word definition
void word_exit(){
	//printf("In word_exit()\n");
	//printf("Return stack height is %d\n",returnStack->top);

	// Restore earlier context
	cmdbuf->size = (int) pop(returnStack);
	cmdbuf->array = (codeword_t **) pop(returnStack);
	cmdbuf->ip = (int) pop(returnStack);
	//printf("word_exit() popped return IP: %d cmdbuf->array: %p\n", (int) cmdbuf->ip, (void*)cmdbuf->array);

	return; // to word_next() or a loop coreword
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


void add_cw_to_def_old(codeword_t * cw){
	CHECK_CAP_CODE
	newWordCodeOld[newWordCodeLenOld++] = cw;
	CHECK_CAP_CODE
	newWordCodeOld[newWordCodeLenOld] = NULL;
}
