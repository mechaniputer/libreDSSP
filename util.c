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
		}else if(!strcmp("VAR", array[i]->name)){
			// VAR is followed by a char * containing a variable name
			i++;
			printf("%d) %p: STR %s\n", i, (void*) &array[i], (char *) array[i]);
		}else if(!strcmp("PUSHVAR", array[i]->name) || !strcmp("!", array[i]->name)){
			// PUSHVAR/! are followed by a pointer to a variable struct
			i++;
			printf("%d) %p: VAR %s\n", i, (void*) &array[i], ((variable_t *) array[i])->name);
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
		debug();
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

/*

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

// Depending on whether we are in compile mode, emits to the appropriate buffer.
void emit(codeword_t * cw){
	if(cmdbuf->status & STAT_INC_COMPILE){
		add_cw_to_def_old(cw);
	}else{
		add_cw_to_cmdbuf(cmdbuf, cw);
	}
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
			if(newWordTextOld != NULL){
				CHECK_CAP_TEXT(1)
				if (ch == '\0'){
					newWordTextOld[newWordTextLenOld++] = '\n';
				}else{
					newWordTextOld[newWordTextLenOld++] = ch;
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
				//printf("String complete: %s\n",statement);

				if(cmdbuf->status & STAT_INC_PRINT){
					printf("Emitting print codewords for string\n");
					cmdbuf->status &= (~STAT_INC_PRINT);

					codeword_t * cw_push_literal = coreSearch("PUSHLIT", vocab);
					codeword_t * st_addr = (codeword_t *) statement;
					codeword_t * st_len = (codeword_t *) strlen(statement);
					codeword_t * cw_print_st = coreSearch("TOS", vocab);
					if((cw_print_st == NULL) || (cw_push_literal == NULL)){
						ERR_MISSING_CORE
					}
					emit(cw_push_literal);
					emit(st_addr);
					emit(cw_push_literal);
					emit(st_len);
					emit(cw_print_st);
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

			if(cmdbuf->status & STAT_BR_NO_ELSE){
				// Every alternating word after a BR should be treated as a condition
				if(cmdbuf->status & STAT_EXPECT_BR_COND){
					cmdbuf->status &= (~STAT_EXPECT_BR_COND); // The current statement is not a branch condition nor ELSE
				}else{
					cmdbuf->status |= STAT_EXPECT_BR_COND; // The current statement should be interpreted as a branch condition (or an ELSE)
				}
			}

			if(isNum(statement)){

				// FIXME We need to detect whether this is a BR condition!
				// If it's a BR condition, put the actual value in the cell.
				// Otherwise do the below.
				if(cmdbuf->status & STAT_EXPECT_BR_COND){
					codeword_t * lit_val = (codeword_t *) atol(statement);
					emit(lit_val);
				}else{
					// Might be one of the single-word core literals
					codeword_t * cw = coreSearch(statement, vocab);
					if(cw != NULL){
						emit(cw);
					}else{
						codeword_t * cw_push_literal = coreSearch("PUSHLIT", vocab);
						codeword_t * lit_val = (codeword_t *) atol(statement);
						emit(cw_push_literal);
						emit(lit_val);
					}
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
				if(newWordNameOld != NULL) free(newWordNameOld);
				if(newWordTextOld != NULL) free(newWordTextOld);
				if(newWordCodeOld != NULL) free(newWordCodeOld);
				newWordCodeOld = malloc(INIT_WORDCODE_CAP * sizeof(codeword_t*));
				newWordCodeCapOld = INIT_WORDCODE_CAP;
				newWordNameOld = NULL;
				newWordTextOld = malloc(INIT_WORDTEXT_CAP * sizeof(char)); // allocate definition text array
				newWordTextCapOld = INIT_WORDTEXT_CAP;
				newWordTextLenOld = 0;
				newWordCodeLenOld = 0;
				newWordDictEntryOld = NULL; // Lookup occurs once we get a name
			}else if(!strcmp(statement, ";")){
				if(!(cmdbuf->status & STAT_INC_COMPILE)){
					ERR_FORB_SEMICOLON
				}
				if(newWordNameOld == NULL){
					ERR_EMPTY_DEF
				}

				// Populate last code element with word_exit() AKA ;S
				codeword_t * dict_entry = coreSearch(";S", vocab);
				if(NULL == dict_entry){
					ERR_EXIT
				}else{
					add_cw_to_def_old(dict_entry);
				}

				cmdbuf->status &= (~STAT_INC_COMPILE);
				printf("Definition of %s complete with status %d\n",newWordNameOld, cmdbuf->status);
				CHECK_CAP_TEXT(1)
				// Remove trailing newlines in definition text
				while(newWordTextLenOld > 0 && newWordTextOld[newWordTextLenOld - 1] == '\n'){
					newWordTextLenOld--;
				}
				newWordTextOld[newWordTextLenOld] = '\0';
				printf("%s\n",newWordTextOld);

				// Populate the dictionary entry
				newWordDictEntryOld->data = (intptr_t) newWordCodeOld;  // Set code array pointer
				newWordDictEntryOld->size = newWordCodeLenOld;
				newWordDictEntryOld->text = newWordTextOld;

				// Check if this was a previously undefined word
				undefined_word_t * entry = undefSearch(newWordNameOld, vocab);
				if(NULL != entry){
					// This word was on someone's wishlist!
					resolve_undefined_word(newWordNameOld, newWordDictEntryOld, vocab);
				}else{
					printf("This was word was not previously referenced.\n");
				}

				// Detach
				newWordCodeOld = NULL;
				newWordTextOld = NULL;
				newWordDictEntryOld = NULL;
			}else if(!strcmp(statement, "GROW") || !strcmp(statement, "USE") || !strcmp(statement, "SHUT") || !strcmp(statement, "ONLY")){
				if(cmdbuf->status & STAT_INC_COMPILE){
					ERR_FORB_SYM_IN_WORD
				}
				cmdbuf->status |= STAT_INC_DICT_OP;
				// Get the codeword for this operation
				codeword_t * dict_entry = coreSearch(statement, vocab);
				// Emit codeword to command buffer. We will update the data field later once we have the dictionary name.
				add_cw_to_cmdbuf(cmdbuf, dict_entry);
			}else if(!strcmp(statement, "VAR")){
				if(cmdbuf->status & STAT_INC_VAR_DECL){
					fprintf(stderr,"ERROR: Nested VAR declaration\n");
					ERR_FATAL
				}else if(cmdbuf->status & STAT_INC_VAR_ASGN){
					fprintf(stderr,"ERROR: VAR declaration inside assignment\n");
					ERR_FATAL
				}
				cmdbuf->status |= STAT_INC_VAR_DECL;
			}else if(cmdbuf->status & STAT_INC_VAR_DECL){
				cmdbuf->status &= (~STAT_INC_VAR_DECL);
				// Found the name to declare
				// Emit the word
				codeword_t * decl = newVarDecl(statement);
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					add_cw_to_def_old(decl);
				}else{
					add_cw_to_cmdbuf(cmdbuf, decl);
				}
			}else if(!strcmp(statement, "!")){
				if(cmdbuf->status & STAT_INC_VAR_DECL){
					fprintf(stderr,"ERROR: Nested VAR declaration\n");
					ERR_FATAL
				}else if(cmdbuf->status & STAT_INC_VAR_ASGN){
					fprintf(stderr,"ERROR: VAR declaration inside assignment\n");
					ERR_FATAL
				}
				cmdbuf->status |= STAT_INC_VAR_ASGN;
			}else if(cmdbuf->status & STAT_INC_VAR_ASGN){
				cmdbuf->status &= (~STAT_INC_VAR_ASGN);
				// Got the variable name. Check if it exists.
				char * varName = malloc((1+strlen(statement))*sizeof(char));
				strcpy(varName, statement);
				// Make sure it's not already used by a variable
				if(2 != growSearch(varName, vocab)){
					fprintf(stderr,"ERROR: No such variable %s exists\n",varName);
					ERR_FATAL
				}
				variable_t * destvar = varSearch(varName, vocab);
				codeword_t * asgn = newVarAsgn(destvar);
				if(cmdbuf->status & STAT_INC_COMPILE){
					add_cw_to_def_old(asgn);
				}else{
					add_cw_to_cmdbuf(cmdbuf, asgn);
				}
			}else if(!strcmp(statement, "DO")){
				cmdbuf->status |= STAT_INC_DO_LOOP;
				codeword_t *dict_entry = coreSearch("DO", vocab);
				if(dict_entry == NULL){
					ERR_MISSING_CORE
				}
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					add_cw_to_def_old(dict_entry);
				}else{
					add_cw_to_cmdbuf(cmdbuf, dict_entry);
				}
			}else if(!strcmp(statement, "RP")){
				cmdbuf->status |= STAT_INC_RP_LOOP;
				codeword_t *dict_entry = coreSearch("RP", vocab);
				if(dict_entry == NULL){
					ERR_MISSING_CORE
				}
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					add_cw_to_def_old(dict_entry);
				}else{
					add_cw_to_cmdbuf(cmdbuf, dict_entry);
				}
			}else if(!strcmp(statement, "BR")){
				cmdbuf->status |= STAT_BR_NO_ELSE;
				codeword_t *dict_entry = coreSearch("BR", vocab);
				if(dict_entry == NULL){
					ERR_MISSING_CORE
				}
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					add_cw_to_def_old(dict_entry);
				}else{
					add_cw_to_cmdbuf(cmdbuf, dict_entry);
				}
			}else if((cmdbuf->status & STAT_BR_NO_ELSE) && !strcmp(statement, "ELSE")){
				//printf("Found an ELSE\n");
				assert(cmdbuf->status & STAT_EXPECT_BR_COND); // If we see an ELSE, we were also expecting a condition
				cmdbuf->status &= (~STAT_BR_NO_ELSE);
				cmdbuf->status &= (~STAT_EXPECT_BR_COND);
				cmdbuf->status |= STAT_BR_ELSE;
				codeword_t *dict_entry = coreSearch("ELSE", vocab);
				if(dict_entry == NULL){
					ERR_MISSING_CORE
				}
				// Emit to cmdbuf or word array
				if(cmdbuf->status & STAT_INC_COMPILE){
					add_cw_to_def_old(dict_entry);
				}else{
					add_cw_to_cmdbuf(cmdbuf, dict_entry);
				}
			}else if(cmdbuf->status & STAT_INC_DICT_OP){
				cmdbuf->status &= (~STAT_INC_DICT_OP);
				// The codeword that we need to update is the last one we emitted.
				codeword_t * last_cw = cmdbuf->array[cmdbuf->size - 1];
				// We won't do the lookup here. Instead we attach the dictionary name as text. The coreword will handle the rest.
				last_cw->text = malloc((1+strlen(statement))*sizeof(char));
				strcpy(last_cw->text, statement);
				// TODO Put string pointer into command/wordCode body instead of using codeWord field
				//      Modify dictionary ops to fetch operands from code body
			}else if(cmdbuf->status & STAT_INC_COMPILE){
				if(newWordNameOld == NULL){
					// We just found a name for the new definition
					newWordNameOld = malloc((1+strlen(statement))*sizeof(char));
					strcpy(newWordNameOld, statement);
					// Make sure it's not already used by a variable
					if(2 == growSearch(newWordNameOld, vocab)){
						ERR_NAME_CONFLICT
					}
					newWordDictEntryOld = wordDefine(newWordNameOld, vocab);

					// newWordCode already allocated in compile mode setup
					// Populate start of text entry
					CHECK_CAP_TEXT(strlen(newWordNameOld)+3)
					strcpy(newWordTextOld, ": ");
					strcat(newWordTextOld, newWordNameOld);
					strcat(newWordTextOld, " ");
					newWordTextLenOld = strlen(newWordTextOld);
				}else{
					// Add word to ongoing word definition
					variable_t * var_lookup = NULL;
					codeword_t *dict_entry = coreSearch(statement, vocab);
					if(NULL != dict_entry){ // Found in core dictionary
						add_cw_to_def_old(dict_entry);
						// If this finishes a BR-ELSE, reset that status
						if(cmdbuf->status & STAT_BR_ELSE){
							printf("Found ELSE condition (coreword) inside compiled word\n");
							cmdbuf->status &= (~STAT_BR_ELSE);
						}else if(cmdbuf->status & STAT_INC_DO_LOOP){
							// If this finishes a DO, reset that status
							printf("Found core word %s inside DO loop\n", statement);
							cmdbuf->status &= (~STAT_INC_DO_LOOP);
							// Emit LOOP codeword after the single-word loop body
							codeword_t *loop_cw = coreSearch("DO_LOOP", vocab);
							if(loop_cw == NULL){
								ERR_MISSING_CORE
							}
							add_cw_to_def_old(loop_cw);
						}else if(cmdbuf->status & STAT_INC_RP_LOOP){
							// If this finishes a RP, reset that status
							printf("Found core word %s inside RP loop\n", statement);
							cmdbuf->status &= (~STAT_INC_RP_LOOP);
							// Emit RP_LOOP codeword after the single-word loop body
							codeword_t *loop_cw = coreSearch("RP_LOOP", vocab);
							if(loop_cw == NULL){
								ERR_MISSING_CORE
							}
							add_cw_to_def_old(loop_cw);
						}
					}else{ // Not found in core dictionary
						dict_entry = wordSearch(statement, vocab);
						var_lookup = varSearch(statement, vocab);
						// Emit the pointer to codeword_t for user word
						if(NULL != dict_entry){
							add_cw_to_def_old(dict_entry);
							// If this finishes a BR-ELSE, reset that status
							if(cmdbuf->status & STAT_BR_ELSE){
								printf("Found ELSE condition (user word) inside compiled word\n");
								cmdbuf->status &= (~STAT_BR_ELSE);
							}else if(cmdbuf->status & STAT_INC_DO_LOOP){
								// If this finishes a DO, reset that status
								printf("Found user word %s inside DO loop\n", statement);
								cmdbuf->status &= (~STAT_INC_DO_LOOP);
								// Emit LOOP codeword after the single-word loop body
								codeword_t *loop_cw = coreSearch("DO_LOOP", vocab);
								if(loop_cw == NULL){
									ERR_MISSING_CORE
								}
								add_cw_to_def_old(loop_cw);
							}else if(cmdbuf->status & STAT_INC_RP_LOOP){
								// If this finishes a DO, reset that status
								printf("Found user word %s inside DO loop\n", statement);
								cmdbuf->status &= (~STAT_INC_RP_LOOP);
								// Emit LOOP codeword after the single-word loop body
								codeword_t *loop_cw = coreSearch("RP_LOOP", vocab);
								if(loop_cw == NULL){
									ERR_MISSING_CORE
								}
								add_cw_to_def_old(loop_cw);
							}
						}else if(NULL != var_lookup){
							// FIXME emit generic pushaddr word followed by address of variable data
							// Emit pointer to codeword invoking var push routine
							codeword_t * cw = newVarPush(var_lookup);
							if(cw == NULL){
								ERR_MISSING_CORE
							}
							add_cw_to_def_old(cw);


							// If this finishes a BR-ELSE, reset that status
							if(cmdbuf->status & STAT_BR_ELSE){
								printf("Found ELSE condition (var push) inside compiled word\n");
								cmdbuf->status &= (~STAT_BR_ELSE);
							}else if(cmdbuf->status & STAT_INC_DO_LOOP){
								// If this finishes a DO, reset that status
								printf("Found var push %s inside DO loop\n", statement);
								cmdbuf->status &= (~STAT_INC_DO_LOOP);
								// Emit LOOP codeword after the single-word loop body
								codeword_t *loop_cw = coreSearch("DO_LOOP", vocab);
								if(loop_cw == NULL){
									ERR_MISSING_CORE
								}
								add_cw_to_def_old(loop_cw);
							}else if(cmdbuf->status & STAT_INC_RP_LOOP){
								// If this finishes a RP, reset that status
								printf("Found var push %s inside RP loop\n", statement);
								cmdbuf->status &= (~STAT_INC_RP_LOOP);
								// Emit LOOP codeword after the single-word loop body
								codeword_t *loop_cw = coreSearch("RP_LOOP", vocab);
								if(loop_cw == NULL){
									ERR_MISSING_CORE
								}
								add_cw_to_def_old(loop_cw);
							}

						}else{
							printf("Unknown word %s found in word definition\n",statement);
							// Check for the word in the undefined words list
							undefined_word_t* entry = undefSearch(statement, vocab);
							// If it's not there yet, add it.
							if(NULL == entry){
								entry = create_undefined_word(statement, vocab);
							}
							add_reference(entry, newWordDictEntryOld);
							// Emit a ref.
							add_cw_to_def_old(entry->placeholder);

							if(cmdbuf->status & STAT_BR_ELSE){
								printf("Found ELSE condition (coreword) inside compiled word\n");
								cmdbuf->status &= (~STAT_BR_ELSE);
							}else if(cmdbuf->status & STAT_INC_DO_LOOP){
								// If this finishes a DO, reset that status
								printf("This word completed a DO loop at the end of a word\n");
								cmdbuf->status &= (~STAT_INC_DO_LOOP);
								// Emit LOOP codeword after the single-word loop body
								codeword_t *loop_cw = coreSearch("DO_LOOP", vocab);
								if(loop_cw == NULL){
									ERR_MISSING_CORE
								}
								add_cw_to_def_old(loop_cw);
							}else if(cmdbuf->status & STAT_INC_RP_LOOP){
								// If this finishes a RP, reset that status
								printf("This word completed a RP loop at the end of a word\n");
								cmdbuf->status &= (~STAT_INC_RP_LOOP);
								// Emit LOOP codeword after the single-word loop body
								codeword_t *loop_cw = coreSearch("RP_LOOP", vocab);
								if(loop_cw == NULL){
									ERR_MISSING_CORE
								}
								add_cw_to_def_old(loop_cw);
							}
						}
					}
				}
			}else{
				// Normal execution mode - emit word to cmdbuf
				codeword_t *dict_entry = coreSearch(statement, vocab);
				variable_t * var_lookup = NULL;
				if(NULL != dict_entry){
					add_cw_to_cmdbuf(cmdbuf, dict_entry);
					if(cmdbuf->status & STAT_BR_ELSE){
						cmdbuf->status &= (~STAT_BR_ELSE);
					}else if(cmdbuf->status & STAT_INC_DO_LOOP){
						// If this finishes a DO, reset that status
						printf("Found core word %s inside DO loop\n", statement);
						cmdbuf->status &= (~STAT_INC_DO_LOOP);
						// Emit LOOP codeword after the single-word loop body
						codeword_t *loop_cw = coreSearch("DO_LOOP", vocab);
						if(loop_cw == NULL){
							ERR_MISSING_CORE
						}
						add_cw_to_cmdbuf(cmdbuf, loop_cw);
					}else if(cmdbuf->status & STAT_INC_RP_LOOP){
						// If this finishes a DO, reset that status
						printf("Found core word %s inside RP loop\n", statement);
						cmdbuf->status &= (~STAT_INC_RP_LOOP);
						// Emit LOOP codeword after the single-word loop body
						codeword_t *loop_cw = coreSearch("RP_LOOP", vocab);
						if(loop_cw == NULL){
							ERR_MISSING_CORE
						}
						add_cw_to_cmdbuf(cmdbuf, loop_cw);
					}
				}else{
					dict_entry = wordSearch(statement, vocab);
					if(NULL != dict_entry){
						add_cw_to_cmdbuf(cmdbuf, dict_entry);
						if(cmdbuf->status & STAT_BR_ELSE){
							cmdbuf->status &= (~STAT_BR_ELSE);
						}else if(cmdbuf->status & STAT_INC_DO_LOOP){
							// If this finishes a DO, reset that status
							printf("Found user word %s inside DO loop\n", statement);
							cmdbuf->status &= (~STAT_INC_DO_LOOP);
							// Emit LOOP codeword after the single-word loop body
							codeword_t *loop_cw = coreSearch("DO_LOOP", vocab);
							if(loop_cw == NULL){
								ERR_MISSING_CORE
							}
							add_cw_to_cmdbuf(cmdbuf, loop_cw);
						}else if(cmdbuf->status & STAT_INC_RP_LOOP){
							// If this finishes a DO, reset that status
							printf("Found user word %s inside RP loop\n", statement);
							cmdbuf->status &= (~STAT_INC_RP_LOOP);
							// Emit LOOP codeword after the single-word loop body
							codeword_t *loop_cw = coreSearch("RP_LOOP", vocab);
							if(loop_cw == NULL){
								ERR_MISSING_CORE
							}
							add_cw_to_cmdbuf(cmdbuf, loop_cw);
						}
					}else{
						var_lookup = varSearch(statement, vocab);
						if(NULL != var_lookup){
							codeword_t * cw = newVarPush(var_lookup);
							if(cw == NULL){
								ERR_MISSING_CORE
							}
							add_cw_to_cmdbuf(cmdbuf, cw);
						}
					}
				}
				if((NULL == dict_entry) && (NULL == var_lookup)){
					// No point in using undef table since this is live execution.
					printf("%s not known\n",statement);
				}
			}

			statement_len = 0; // No need to get a new buffer since we didn't detach it
		}
	}while(ch != '\0');

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
*/
