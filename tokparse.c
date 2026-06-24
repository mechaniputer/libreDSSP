#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>


#include "util.h"
#include "dict.h"
#include "stack.h"
#include "tokparse.h"

extern dict * vocab;
extern cmdbuffer * cmdbuf;
extern int abort_requested;

// Global singleton codewords
extern codeword_t cw_var_undef_assign;
extern codeword_t cw_var_generic_assign;
extern codeword_t cw_var_undef_addrof;
extern codeword_t cw_var_generic_addrof;
extern codeword_t cw_var_undef_set0;
extern codeword_t cw_var_generic_set0;
extern codeword_t cw_var_undef_set1;
extern codeword_t cw_var_generic_set1;

// Tokenizer globals
extern stack *tokenizerStack;
static char *scan_ptr = NULL;
intptr_t tokenizer_state = TOK_NORMAL;
char * tokenBuffer;
int token_cap;
int token_len;

// Parser globals
extern stack *parserStack;
intptr_t parser_state = PARSE_NORMAL;
int compiling; // 0-emit to cmdbuf 1-emit to newWordDef

// Globals to define word across potentially several parser invocations
// TODO Package into struct and create helpers
char *newWordName;
char *newWordText;
int newWordTextLen;
int newWordTextCap;
codeword_t **newWordCode;
int newWordCodeLen;
int newWordCodeCap;
codeword_t * newWordDictEntry;
pend_undef_ref_t * newWordUndefs;
int newWordPendUndefsLen;
int newWordPendUndefsCap;


// For ensuring that we do not emit a multicell word when we only have one cell to populate (eg, in a conditional or loop)
static const char *const multicell_words[] = {"IF-","IF0","IF+","BR-","BR0","BR+","BRS","BR","DO","RP","VAR","!","GROW","SHUT","USE","SEE","DEL", "!"};
static const size_t NUM_MULTICELL_WORDS = sizeof(multicell_words) / sizeof(multicell_words[0]);

#define PARSE_ENTER_STATE(ST)  { push(parserStack, parser_state); parser_state = ST; }
#define PARSE_CHANGE_STATE(ST) { parser_state = ST; }
#define PARSE_EXIT_STATE       { parser_state = pop(parserStack); }

#define INIT_STATEMENT_CAP (8)
#define INIT_WORDCODE_CAP (16)
#define INIT_WORDTEXT_CAP (32)

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



// NOTE: If an aborted definition contained refs to a string literal (eg for
// printing) then those refs will be lost and the buffer will be leaked.
// This should be fixed eventually by keeping a list of attached strings until
// a definition is complete.
void reset_tokenizer_parser_state(){
	if(compiling){
		if(newWordDictEntry != NULL){
			//A word definition was in progress and already had a dictionary entry.
			// Replace prior definition with stub ;S definition so execution and undef lookups are safe
			// We reuse the existing codeword_t since it is already in the dictionary
			codeword_t ** stubDef = malloc(2*sizeof(codeword_t *));
			stubDef[0] = coreSearch(";S");
			stubDef[1] = NULL;

			// Prevent leaks
			if(newWordDictEntry->data != 0){
				free((void *) newWordDictEntry->data);
				newWordDictEntry->data = 0;
			}
			if(newWordDictEntry->text != 0){
				free((void *) newWordDictEntry->text);
				newWordDictEntry->data = 0;
			}

			// Populate the dictionary entry
			newWordDictEntry->data = (intptr_t) stubDef;  // Set code array pointer
			newWordDictEntry->size = 1;
			char * temp_st = " [aborted definition]";
			int len1 = strlen(temp_st);
			int len2 = strlen(newWordDictEntry->name);
			newWordDictEntry->text = malloc(len1 + len2 + 1);
			strcpy(newWordDictEntry->text, newWordDictEntry->name);
			strcpy(newWordDictEntry->text + len2, temp_st);
			newWordDictEntry = NULL;
			// Do not free newDictEntry! It's in the dictionary now.
		}
		compiling = 0;
	}
	if(newWordName != NULL) free(newWordName);
	if(newWordText != NULL) free(newWordText);
	if(newWordCode != NULL) free(newWordCode);
	newWordName = NULL;
	newWordText = NULL;
	newWordCode= NULL;
	parser_state = PARSE_NORMAL;
	tokenizer_state = TOK_NORMAL;
	parserStack->top = -1;
	tokenizerStack->top = -1;
	if(tokenBuffer != NULL) free(tokenBuffer);
	tokenBuffer = NULL;
	token_len = 0;
	token_cap = 0;
	scan_ptr = NULL;
	reset_pending_undefs();
	return;
}

void tokenAppendChar(char ch){
	if(tokenBuffer == NULL){
		// If there isn't a token buffer, make one
		tokenBuffer = malloc(4*sizeof(char));
		token_cap = 4;
		token_len = 0;
	}else if(token_len+1 == token_cap){
		// Check capacity
		token_cap+=4;
		tokenBuffer = realloc(tokenBuffer, token_cap*sizeof(char));
		assert(tokenBuffer!= NULL);
	}
	tokenBuffer[token_len++] = ch;
	tokenBuffer[token_len] = '\0';
}

// Resets token buffer and returns finished token
char * endToken(){
	if(token_len == 0) return NULL;
	char * temp = tokenBuffer;
	tokenBuffer = NULL;
	token_len = 0;
	token_cap = 0;
	return temp;
}

// Call to start new line (does not clear tokenizer state)
void tokenizer_start_line(char *line) {
	scan_ptr = line;
}

// Return next token from line (or NULL)
// Filters out [comments]
char *get_next_token() {
	// If no line is here to parse
	if (!scan_ptr) return NULL;

	//printf("Tokenizer begins in state %d\n",tokenizer_state);

	// Each iteration processes a single ch.
	// This is slow, but simple.
	// The loop goes until a return happens.
	for (;;) {
		if(abort_requested) return NULL;
		switch(tokenizer_state){
		case TOK_NORMAL:
			if(*scan_ptr == '\0'){
				// Reached the end of the line. Do we have something to return?
				if(token_len!=0){
					return endToken();
				}else{
					return NULL;
				}
			}else if(isspace(*scan_ptr)){
				// Did we just finish a token in the previous iteration?
				if(token_len!=0){
					return endToken();
				}
			}else if (*scan_ptr == '[') {
				tokenAppendChar(*scan_ptr);
				push(tokenizerStack, tokenizer_state);
				tokenizer_state = TOK_COMMENT;
			}else if (*scan_ptr == '\"') {
				tokenAppendChar(*scan_ptr);
				push(tokenizerStack, tokenizer_state);
				tokenizer_state = TOK_STRING;
			}else if(!isspace(*scan_ptr)){
				tokenAppendChar(*scan_ptr);
			}
			break;


		case TOK_COMMENT:
			// We don't filter comments so that, at least, inline comments like stack annotations make it into definition text.
			if(*scan_ptr == '\0'){
				// The line ended and we are still in a (multi-line) comment.
				return NULL;
			}else if(*scan_ptr == ']'){
				tokenizer_state = pop(tokenizerStack);
			}else if(*scan_ptr == '\\'){
				tokenizer_state = TOK_COMMENT_ESC;
			}
			tokenAppendChar(*scan_ptr);
			break;

		case TOK_COMMENT_ESC: // Read one character and don't pop the status, even if it's ]
			if(*scan_ptr == '\0'){ // Cannot escape \0
				return NULL;
			}
			tokenAppendChar(*scan_ptr);
			tokenizer_state = TOK_COMMENT;
			break;

		case TOK_STRING:
			if(*scan_ptr == '\0'){
				return NULL;
			}else if(*scan_ptr == '\n'){
				// Newlines get replaced with spaces
				tokenAppendChar(' ');
			}else if(*scan_ptr == '\"'){
				// A quote ends the string
				tokenAppendChar(*scan_ptr);
				tokenizer_state = pop(tokenizerStack);
			}else if(*scan_ptr == '\\'){
				tokenizer_state = TOK_STRING_ESC;
			}else{
				tokenAppendChar(*scan_ptr);
			}
			break;

		case TOK_STRING_ESC: // This is here to allow us to put double quotes in strings
			if(*scan_ptr == '\0'){ // Cannot escape \0
				return NULL;
			}
			tokenAppendChar(*scan_ptr);
			tokenizer_state = TOK_STRING;
			break;

		default:
			printf("Fatal Error: Unknown tokenizer state\n");
			exit(-1); // This is not an error we should recover from as it indicates a problem in libreDSSP itself
			break;
		}
		scan_ptr++;
	}
}

// If it's a string, allocate a buffer and strip the quotes
char * isString(char * tok){
	int len = strlen(tok);
	if(strlen(tok) >= 2){
		if((tok[0] == '\"') && (tok[len-1] == '\"')){
			char * st = malloc((len+1) * sizeof(char));
			int toklen = strlen(tok);
			strncpy(st, tok+1, toklen-2);
			st[toklen-2] = '\0';
			return st;
		}
	}
	return NULL;
}

// If it's a print, allocate a buffer and strip the quotes and dot
char * isPrint(char * tok){
	int len = strlen(tok);
	if(strlen(tok) >=3){
		if((tok[0] == '.') && (tok[1] == '\"') && (tok[len-1] == '\"')){
			char * st = malloc((len+1) * sizeof(char));
			int toklen = strlen(tok);
			strncpy(st, tok+2, toklen-3);
			st[toklen-3] = '\0';
			return st;
		}
	}
	return NULL;
}

// Returns 1 if it's a valid dictionary name, otherwise 0.
int isDict(const char *s) {
	if (!s || *s != '$') return 0;
	if(!strcmp("$PRIME", s)) return 0; // Cannot touch $PRIME
	s++;
	if (!isalpha((unsigned char)*s)) return 0;
	for (; *s; s++) {
		if (!isalpha((unsigned char)*s)) return 0;
	}
	return 1;
}

// Returns 1 if it's a valid word/var name, otherwise 0.
// Word/var names must be alphanumeric, contain at least one letter, and may contain underscores.
int isValidWordVarName(const char *s) {
	int has_letter = 0;
	if (!s || !*s) return 0;
	for (; *s; s++) {
		unsigned char c = (unsigned char)*s;
		if (isalpha(c)) has_letter = 1;
		else if (isdigit(c) || c == '_') continue;
		else return 0;
	}
	return has_letter;
}

// If the word is a multi-cell core word, trigger an abort.
// Necessary when compiling branches, conditionals, and loops.
// FIXME Consider a more efficient approach (eg hashing or use the size field
//       in core word entries to indicate number of cells needed)
void ensureSingleCellWord(const char * s){
	for(int i=0; i<NUM_MULTICELL_WORDS; i++){
		if(!strcmp(s, multicell_words[i])){
			printf("ERR: %s not allowed as a branch/loop word\n",s);
			abortExecution();
			return;
		}
	}
	return;
}

// Resets to empty, frees buffers
void reset_pending_undefs(){
	if(newWordUndefs != NULL){
		free(newWordUndefs);
	}
	newWordUndefs = NULL;
	newWordPendUndefsLen = 0;
	newWordPendUndefsCap = 0;
}

// Appends a new undefined ref
// Grows in increments of 2 elements
void add_pending_undef(undefined_ref_t * uref, int cell_index){
	//printf("Adding pending undef: %s at cell %d\n", uref->name, cell_index);
	if(newWordUndefs == NULL){
		newWordUndefs = malloc(2*sizeof(pend_undef_ref_t));
		newWordPendUndefsCap = 2;
	}else if(newWordPendUndefsLen == newWordPendUndefsCap){
		newWordPendUndefsCap += 2;
		newWordUndefs = realloc(newWordUndefs, newWordPendUndefsCap * sizeof(pend_undef_ref_t));
	}
	newWordUndefs[newWordPendUndefsLen].uref = uref;
	newWordUndefs[newWordPendUndefsLen].cell_index = cell_index;
	newWordPendUndefsLen += 1;
}

void start_new_def(){
	// Prepare vars to build definition
	if(newWordName != NULL) free(newWordName);
	if(newWordText != NULL) free(newWordText);
	if(newWordCode != NULL) free(newWordCode);
	newWordCode = malloc(INIT_WORDCODE_CAP * sizeof(codeword_t*));
	newWordCodeCap = INIT_WORDCODE_CAP;
	newWordName = NULL;
	newWordText = malloc(INIT_WORDTEXT_CAP * sizeof(char)); // allocate definition text array
	newWordText[0] = '\0';
	newWordTextCap = INIT_WORDTEXT_CAP;
	newWordTextLen = 0;
	newWordCodeLen = 0;
	newWordDictEntry = NULL; // Lookup occurs once we get a name
	reset_pending_undefs();
}

void add_cw_to_def(codeword_t * cw){
	CHECK_CAP_CODE
	newWordCode[newWordCodeLen++] = cw;
	CHECK_CAP_CODE
	newWordCode[newWordCodeLen] = NULL;
}

// Depending on whether we are in compile mode, emits to the appropriate buffer.
void emit_cw(codeword_t * cw){
	//printf("emit(%ld)\n",(intptr_t) cw);
	if(compiling){
		add_cw_to_def(cw);
	}else{
		add_cw_to_cmdbuf(cmdbuf, cw);
	}
}

// Emits a word reference one way or another
// Tries core, then user, then undef, and then lastly makes a new undef
void emit_word_by_name(char * name){
	codeword_t * dict_entry = NULL;
	undefined_ref_t * uref = NULL;
	if((dict_entry = coreSearch(name)) != NULL){
		// Core word
		emit_cw(dict_entry);
	}else if((dict_entry = wordSearch(name)) != NULL){
		/// User word
		emit_cw(dict_entry);
	}else if(compiling && (uref = undefSearch(name)) != NULL){
		// Previously known undef word
		// Don't add a ref if not in compiling mode! It will be a NULL ref!
		add_pending_undef(uref, newWordCodeLen); // Record the pending undef ref with the current cell index
		//add_reference(uref, newWordDictEntry); // Make the undef word record point here
		emit_cw(uref->ref_placeholder);
	}else if(compiling){
		// New undef word
		if(!isValidWordVarName(name)){
			printf("ERR: Symbol %s is not a valid word name\n", name);
			abortExecution();
			return;
		}
		uref = create_undefined_ref(name);
		add_pending_undef(uref, newWordCodeLen); // Record the pending undef ref with the current cell index
		//add_reference(uref, newWordDictEntry); // Make the undef word record point here
		emit_cw(uref->ref_placeholder);
	}else{
		// We are not compiling, so whatever we emit will be run immediately.
		// There's no point in emitting anything undefined.
		printf("ERR: Symbol %s undefined\n", name);
		abortExecution();
		return;
	}
}

// Takes one token at a time.
// Updates the parse state while emitting code to the appropriate buffer
// Return 0: good
// Return -1: Error, must reset the parser
int parse_tokens(char * tok){
	//printf("Parse state %ld\n",parser_state);
	codeword_t * dict_entry = NULL;
	variable_t * var_lookup = NULL;
	undefined_ref_t * uref = NULL;
	subdict * sub = NULL;
	char * st; // For string buffers
	char * namebuf; // For buffers that we detach into the dictionary

	// If we are in compile mode, record the token in the word definition text.
	if(compiling){
		CHECK_CAP_TEXT(strlen(tok)+1)
		if(newWordTextLen != 0){
			strcat(newWordText, " ");
			newWordTextLen += 1;
		}
		strcat(newWordText, tok);
		newWordTextLen += strlen(tok);
	}

	// If it's a comment, then our work is already done here.
	if(tok[0] == '[') return 0;

	switch(parser_state){
	case PARSE_COMPILE_S0:
		// Next token should be a valid user word name (alphanumeric, not in core or vars)
		if(!isValidWordVarName(tok)){
			printf("ERR: Word name %s not allowed\n",tok);
			abortExecution();
			return 0;
		}else if (coreSearch(tok)){
			printf("ERR: %s is a core word\n",tok);
			abortExecution();
			return 0;
		}else if(varSearch(tok)){
			printf("ERR: %s is a variable\n",tok);
			abortExecution();
			return 0;
		}else if(referenced_as_var(tok)){
			printf("ERR: %s is referenced as a variable\n",tok);
			abortExecution();
			return 0;
		}else{
			newWordName = malloc((1+strlen(tok))*sizeof(char));
			strcpy(newWordName, tok);

			// wordDefine is able to request an abort so we need to watch for that here
			newWordDictEntry = wordDefine(newWordName);
			if(abort_requested) return 0; // Parser has already been reset

			// newWordCode already allocated in compile mode setup
			// Populate start of text entry
			CHECK_CAP_TEXT(strlen(newWordName)+3)
			strcpy(newWordText, ": ");
			strcat(newWordText, newWordName);
			strcat(newWordText, " ");
			newWordTextLen = strlen(newWordText);
			PARSE_CHANGE_STATE(PARSE_COMPILE_S1);
		}
		break;
	case PARSE_COMPILE_S1:
		if(!strcmp(tok,";")){
			emit_cw(coreSearch(";S"));
			//printf("Definition of %s complete\n", newWordName);

			// We might be redefining a prior word. This prevents a leak.
			if(newWordDictEntry->data != 0){
				untrack_undef_refs((codeword_t **)newWordDictEntry->data);
				free((void *) newWordDictEntry->data);
				newWordDictEntry->data = 0;
			}
			if(newWordDictEntry->text != 0){
				free((void *) newWordDictEntry->text);
				newWordDictEntry->data = 0;
			}

			// Populate the dictionary entry
			newWordDictEntry->data = (intptr_t) newWordCode;  // Set code array pointer
			newWordDictEntry->size = newWordCodeLen;
			newWordDictEntry->text = newWordText;

			// Finalize undef refs made within the new word
			for(int i=0; i < newWordPendUndefsLen; i++){
				uref = newWordUndefs[i].uref;
				int cell_index = newWordUndefs[i].cell_index;
				add_reference(uref, &(newWordCode[cell_index]));
			}
			reset_pending_undefs();

			// Check if this was a previously undefined word
			undefined_ref_t * entry = undefSearch(newWordName);
			if(NULL != entry){
				resolve_undefined_ref(newWordName, newWordDictEntry, /*isVar*/ 0, /*var*/ NULL);
			}
			//print_codewords(newWordCode);
			// Detach
			newWordCode = NULL;
			newWordText = NULL;
			newWordDictEntry = NULL;

			PARSE_EXIT_STATE
			compiling = 0;
			break;
		}else if(!strcmp(tok,":")){
			printf("ERR: Definitions cannot be nested\n");
			abortExecution();
			return 0;
		}
		// FALL-THROUGH
	case PARSE_NORMAL:
		// In this mode, we expect:
		// core words, user words, undef words, literal numbers, prints, strings.
		if((dict_entry = coreSearch(tok)) != NULL){
			// It's a core word
			emit_cw(dict_entry);

			// STATUS UPDATES
			if(!strncmp(tok,"IF",2)){
				PARSE_ENTER_STATE(PARSE_IF_S0);
			}else if(!strcmp(tok,"BR-") || !strcmp(tok,"BR0") || !strcmp(tok,"BR+")){
				PARSE_ENTER_STATE(PARSE_BR2_S0);
			}else if(!strcmp(tok,"BRS")){
				PARSE_ENTER_STATE(PARSE_BRS_S0);
			}else if(!strcmp(tok,"BR")){
				PARSE_ENTER_STATE(PARSE_BR_S0);
			}else if(!strcmp(tok,"DO")){
				PARSE_ENTER_STATE(PARSE_DO_S0);
			}else if(!strcmp(tok,"RP")){
				PARSE_ENTER_STATE(PARSE_RP_S0);
			}else if(!strncmp(tok,"VAR",3) || !strcmp(tok,"SEE")){
				PARSE_ENTER_STATE(PARSE_VAR_NAME_S0);
			}else if(!strcmp(tok,"GROW")){
				PARSE_ENTER_STATE(PARSE_DICT_NEW);
			}else if(!strcmp(tok,"DEL")){
				PARSE_ENTER_STATE(PARSE_DEL_S0);
			}else if(!strcmp(tok,"SHUT") || !strcmp(tok,"USE")){
				PARSE_ENTER_STATE(PARSE_DICT_EXIST);
			}
		}else if(!strcmp(tok,"!")){
			// We defer emitting the coreword because it might need to be the undef variant.
			PARSE_ENTER_STATE(PARSE_ASGN_S0);
		}else if(!strcmp(tok,"'")){
			// We defer emitting the coreword because it might need to be the undef variant.
			PARSE_ENTER_STATE(PARSE_ADDROF_S0);
		}else if(!strcmp(tok,"!0")){
			// We defer emitting the coreword because it might need to be the undef variant.
			PARSE_ENTER_STATE(PARSE_SET0_S0);
		}else if(!strcmp(tok,"!1")){
			// We defer emitting the coreword because it might need to be the undef variant.
			PARSE_ENTER_STATE(PARSE_SET1_S0);
			// TODO additional state transitions for VAR ops (!1+, !1-, !+, !-, SIZE)
		}else if(!strcmp(tok,":")){
				compiling = 1;
				start_new_def();
				PARSE_ENTER_STATE(PARSE_COMPILE_S0);
		}else if((st = isPrint(tok)) != NULL){
			// Push the string info, and print it
			codeword_t * cw_push_literal = coreSearch("PUSHLIT");
			emit_cw(cw_push_literal);
			emit_cw((codeword_t *) st);
			emit_cw(cw_push_literal);
			emit_cw((codeword_t *) strlen(st));
			emit_cw(coreSearch("TOS"));
		}else if((st = isString(tok)) != NULL){
			// Push the string info, but don't print it
			codeword_t * cw_push_literal = coreSearch("PUSHLIT");
			emit_cw(cw_push_literal);
			emit_cw((codeword_t *) st);
			emit_cw(cw_push_literal);
			emit_cw((codeword_t *) strlen(st));
		}else if(isNum(tok)){
			// It's a number
			emit_cw(coreSearch("PUSHLIT"));
			emit_cw((codeword_t *) atol(tok));
		}else if((var_lookup = varSearch(tok)) != NULL){
			emit_cw(&var_lookup->cw_pushVar);
		}else if (isValidWordVarName(tok)){
			// It must be a user word, possibly undefined
			dict_entry = wordSearch(tok);
			if((dict_entry = wordSearch(tok)) != NULL){
				emit_cw(dict_entry);
			}else if(compiling && ((uref = undefSearch(tok)) != NULL)){
				// Previously known undef word
				add_pending_undef(uref, newWordCodeLen); // Record the pending undef ref with the current cell index
				emit_cw(uref->ref_placeholder);
			}else if(compiling){
				// New undef word
				uref = create_undefined_ref(tok);
				add_pending_undef(uref, newWordCodeLen); // Record the pending undef ref with the current cell index
				emit_cw(uref->ref_placeholder);
			}else{
				// We are not compiling, so whatever we emit will be run immediately.
				// There's no point in emitting anything undefined.
				printf("ERR: Symbol %s undefined\n", tok);
				abortExecution();
				return 0;
			}
		}else{
			printf("ERR: Unexpected %s\n",tok);
			abortExecution();
			return 0;
		}
		break;
	case PARSE_IF_S0:
		// IF-/IF0/IF+ only need one branch target
		// This might be a core word, a user word, or an undefined word (nothing else)
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		PARSE_EXIT_STATE
		break;
	case PARSE_BR2_S0:
		// Expecting first of two branch targets
		// This might be a core word, a user word, or an undefined word (nothing else)
		// But there is no room for a multi-cell word in an BR.
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		emit_cw(coreSearch("SKP1"));
		PARSE_CHANGE_STATE(PARSE_BR2_S1)
		break;
	case PARSE_BR2_S1:
		// Expecting second of two branch targets
		// This might be a core word, a user word, or an undefined word (nothing else)
		// But there is no room for a multi-cell word in an BR.
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		PARSE_EXIT_STATE
		break;
	case PARSE_BRS_S0:
		// Expecting first of three branch targets
		// This might be a core word, a user word, or an undefined word (nothing else)
		// But there is no room for a multi-cell word in an BRS.
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		emit_cw(coreSearch("SKP1"));
		PARSE_CHANGE_STATE(PARSE_BRS_S1)
		break;
	case PARSE_BRS_S1:
		// Expecting second of three branch targets
		// This might be a core word, a user word, or an undefined word (nothing else)
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		emit_cw(coreSearch("SKP1"));
		PARSE_CHANGE_STATE(PARSE_BRS_S2)
		break;
	case PARSE_BRS_S2:
		// Expecting third of three branch targets
		// This might be a core word, a user word, or an undefined word (nothing else)
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		PARSE_EXIT_STATE
		break;
	case PARSE_BR_S0:
		// Expecting first branch condition
		// This has to be an integer
		if(isNum(tok)){
			// It's a number
			emit_cw((codeword_t *) atol(tok));
			PARSE_CHANGE_STATE(PARSE_BR_S2)
		}else{
			printf("Unexpected symbol %s in BR\n", tok);
			abortExecution();
			return 0;
		}
		break;
	case PARSE_BR_S1:
		// Expecting another branch condition (a literal constant) or an ELSE
		// This has to be an integer or an ELSE token
		if(isNum(tok)){
			// It's a number
			emit_cw(coreSearch("SKP2"));
			emit_cw((codeword_t *) atol(tok));
			PARSE_CHANGE_STATE(PARSE_BR_S2)
		}else if(!strcmp("ELSE", tok)){
			// It's an ELSE
			// No emit
			PARSE_CHANGE_STATE(PARSE_BR_S3)
		}else{
			printf("Unexpected symbol %s in BR\n", tok);
			abortExecution();
			return 0;
		}
		break;
	case PARSE_BR_S2:
		// Expecting a branch target
		// This might be a core word, a user word, or an undefined word (nothing else)
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		PARSE_CHANGE_STATE(PARSE_BR_S1)
		break;
	case PARSE_BR_S3:
		// Expecting one final branch target
		// This might be a core word, a user word, or an undefined word (nothing else)
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_cw(coreSearch("SKP1"));
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		PARSE_EXIT_STATE
		break;
	case PARSE_DO_S0:
		// Expecting a word to loop
		// This might be a core word, a user word, or an undefined word (nothing else)
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		emit_cw(coreSearch("DO_LOOP"));
		PARSE_EXIT_STATE
		break;
	case PARSE_RP_S0:
		// Expecting a word to loop
		// This might be a core word, a user word, or an undefined word (nothing else)
		ensureSingleCellWord(tok);
		// ensureSingleCellWord() is able to request an abort so we need to watch for that here
		if(abort_requested){
			return 0;
		}
		emit_word_by_name(tok);
		// emit_word_by_name() is able to request an abort so we need to watch for that here
		if(abort_requested) return 0;
		emit_cw(coreSearch("RP_LOOP"));
		PARSE_EXIT_STATE
		break;
	case PARSE_VAR_NAME_S0:
		// Expecting a new variable name
		// This must not be a core word and must be alphanumeric
		// If we get it, PARSE_EXIT_STATE
		// Otherwise we need to error out of the parser
		if(!isValidWordVarName(tok) || coreSearch(tok)){
			printf("ERR: %s invalid var name\n", tok);
			abortExecution();
			return 0;
		}
		// the VAR/SEE command was already emitted. We just emit a char * here.
		namebuf = malloc((strlen(tok)+1) * sizeof(char));
		strcpy(namebuf, tok);
		emit_cw((codeword_t *) namebuf);
		PARSE_EXIT_STATE
		break;
	case PARSE_DICT_NEW:
		// Expecting a valid new dictionary name
		// Easily checked with isDict().
		if(!isDict(tok)){
			printf("ERR: Cannot name dictionary %s\n",tok);
			abortExecution();
			return 0;
		}
		// the GROW command was already emitted. We just emit a char * here.
		namebuf = malloc((strlen(tok)+1) * sizeof(char));
		strcpy(namebuf, tok);
		emit_cw((codeword_t *) namebuf);
		PARSE_EXIT_STATE
		break;
	case PARSE_DICT_EXIST:
		// Expecting a valid prior dictionary name
		// Easily checked with findDict().
		if((sub = findDict(tok)) == NULL){
			printf("ERR: Unknown dictionary %s\n",tok);
			abortExecution();
			return 0;
		}
		emit_cw((codeword_t *) sub);
		PARSE_EXIT_STATE
		break;
	case PARSE_DEL_S0:
		// Expecting a word/var name to delete (code is similar to PARSE_DICT_NEW)
		// the DEL command was already emitted. We just emit a char * here.
		namebuf = malloc((strlen(tok)+1) * sizeof(char));
		strcpy(namebuf, tok);
		emit_cw((codeword_t *) namebuf);
		PARSE_EXIT_STATE
		break;

	// We stack cases here for compactness.
	// Note that we still need to observe the state to emit the correct code.
	case PARSE_ASGN_S0:
	case PARSE_ADDROF_S0:
	case PARSE_SET0_S0:
	case PARSE_SET1_S0:
		// Can be an undefined or existing variable.
		var_lookup = varSearch(tok);
		if((var_lookup) == NULL){
			if(!compiling){
				// In immediate mode, we error out.
				printf("ERR: Unknown var %s\n",tok);
				abortExecution();
				return 0;
			}else{
				// In compiling mode, we start tracking the undef ref.
				if(!isValidWordVarName(tok)){
					printf("ERR: %s invalid var name\n", tok);
					abortExecution();
					return 0;
				}

				// Check if the name is already a variable
				if(wordSearch(tok)){
					printf("ERR: Symbol %s already defined as a word\n", tok);
					abortExecution();
					return 0;
				}

				// Check if this is a known uref
				uref = undefSearch(tok);
				if(uref == NULL) uref = create_undefined_ref(tok);

				// Add to pending undef refs
				// Note: The pending undef points to the operation cell, not the VAR cell.
				add_pending_undef(uref, newWordCodeLen); // Record the pending undef ref with the current cell index
				if(parser_state == PARSE_ASGN_S0){
					emit_cw(&cw_var_undef_assign);
				}else if(parser_state == PARSE_ADDROF_S0){
					emit_cw(&cw_var_undef_addrof);
				}else if(parser_state == PARSE_SET0_S0){
					emit_cw(&cw_var_undef_set0);
				}else if(parser_state == PARSE_SET1_S0){
					emit_cw(&cw_var_undef_set1);
				}
				emit_cw((codeword_t *) uref->name);
			}
		}else{
			if(parser_state == PARSE_ASGN_S0){
				emit_cw(&cw_var_generic_assign);
			}else if(parser_state == PARSE_ADDROF_S0){
				emit_cw(&cw_var_generic_addrof);
			}else if(parser_state == PARSE_SET0_S0){
				emit_cw(&cw_var_generic_set0);
			}else if(parser_state == PARSE_SET1_S0){
				emit_cw(&cw_var_generic_set1);
			}
			emit_cw((codeword_t*) var_lookup);
		}
		PARSE_EXIT_STATE
		break;
	default:
		printf("Fatal Error: Unknown parser state\n");
		exit(-1); // This is not an error we should recover from as it indicates a problem in libreDSSP itself
		break;
	}
	//printf("Parse state %ld\n",parser_state);
	return 0;
}

int process_line(char * line){
	//printf("*** Tokenizing line: ***\n>> %s\n", line);
	tokenizer_start_line(line);

	while(1){
		char * st = get_next_token();
		if(st == NULL){
			break;
		}else{
			//printf("Parsing token %s\n",st);
			parse_tokens(st);
			free(st);
			// Greedy execution
			if(parser_state == 0){
				word_next();
				cmdbuf->size = 0;
				cmdbuf->array[0] = NULL;
			}
		}
		if(abort_requested){
			return 0; // Keep returning up the chain
		}
	}
	//printf("\n\n");
	return parser_state | tokenizer_state;
}
