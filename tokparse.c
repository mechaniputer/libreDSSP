#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>


#include "util.h"
#include "dict.h"
#include "stack.h"
#include "tokparse.h"

extern dict * vocab;
extern cmdbuffer * cmdbuf;

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
intptr_t * wordDef; // Stand-in for a word definition
int compiling; // 0-emit to cmdbuf 1-emit to newWordDef

// Globals to define word across potentially several parser invocations
char *newWordName;
char *newWordText;
int newWordTextLen;
int newWordTextCap;
codeword_t **newWordCode;
int newWordCodeLen;
int newWordCodeCap;
codeword_t * newWordDictEntry;

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
            // In a comment, we only have to wait for the end of the comment.
            if(*scan_ptr == '\0'){
                return NULL;
            }else if(*scan_ptr == ']'){
                tokenizer_state = pop(tokenizerStack);
            }else if(*scan_ptr == '\\'){
                tokenizer_state = TOK_COMMENT_ESC;
            }
            break;

        case TOK_COMMENT_ESC: // Read one character and don't pop the status, even if it's ]
            if(*scan_ptr == '\0'){ // Cannot escape \0
                return NULL;
            }
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
            assert(0);
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
            strncpy(st, tok+2, toklen-2);
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
int isValidWordVarName(const char *s) {
    if (!isalpha((unsigned char)*s)) return 0;
    for (; *s; s++) {
        if (!isalpha((unsigned char)*s)) return 0;
    }
    return 1;
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
	newWordTextCap = INIT_WORDTEXT_CAP;
	newWordTextLen = 0;
	newWordCodeLen = 0;
	newWordDictEntry = NULL; // Lookup occurs once we get a name
}

void add_cw_to_def(codeword_t * cw){
	CHECK_CAP_CODE
	newWordCode[newWordCodeLen++] = cw;
	CHECK_CAP_CODE
	newWordCode[newWordCodeLen] = NULL;
}

// Depending on whether we are in compile mode, emits to the appropriate buffer.
void emit_cw(codeword_t * cw){
    printf("emit(%ld)\n",(intptr_t) cw);
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
    undefined_word_t * uword = NULL;
    if((dict_entry = coreSearch(name, vocab)) != NULL){
        // Core word
        emit_cw(dict_entry);
    }else if((dict_entry = wordSearch(name, vocab)) != NULL){
        /// User word
        emit_cw(dict_entry);
    }else if((uword = undefSearch(name, vocab)) != NULL){
        // Previously known undef word
        emit_cw(uword->placeholder);
    }else{
        // New undef word
        if(!isValidWordVarName(name)){
            printf("ERR: Unknown symbol %s is not a valid word name", name);
            assert(0);
        }
        uword = create_undefined_word(name, vocab);
        add_reference(uword, newWordDictEntry); // Make the undef word record point here
        emit_cw(uword->placeholder);
    }
}

// Takes one token at a time.
// Updates the parse state while emitting code to the appropriate buffer
// Return 0: good
// Return -1: Error, must reset the parser
// TODO: Currently only undef words are supported, but it would be good to allow undef vars as well
int parse_tokens(char * tok){
    codeword_t * dict_entry = NULL;
    variable_t * var_lookup = NULL;
    undefined_word_t * uword = NULL;
    subdict * sub = NULL;
    char * st; // For string buffers
    char * namebuf; // For buffers that we detach into the dictionary

    // TODO if compiling == 1, add token to word def

    switch(parser_state){
    case PARSE_COMPILE_S0:
        // Next token should be a valid user word name (alphanumeric, not in core or vars)
        if(isValidWordVarName(tok) && (NULL == coreSearch(tok, vocab)) && (NULL == varSearch(tok, vocab))){
            newWordName = malloc((1+strlen(tok))*sizeof(char));
			strcpy(newWordName, tok);
            newWordDictEntry = wordDefine(newWordName, vocab);
            // newWordCode already allocated in compile mode setup
			// Populate start of text entry
			CHECK_CAP_TEXT(strlen(newWordName)+3)
			strcpy(newWordText, ": ");
			strcat(newWordText, newWordName);
			strcat(newWordText, " ");
			newWordTextLen = strlen(newWordText);
            PARSE_CHANGE_STATE(PARSE_COMPILE_S1)
        }else{
            printf("ERR: Word name %s not allowed\n",tok);
            assert(0);
        }
        break;        
    case PARSE_COMPILE_S1:
        if(!strcmp(tok,";")){
            emit_cw(coreSearch(";S", vocab));
            printf("Definition of %s complete\n", newWordName);

            // Populate the dictionary entry
			newWordDictEntry->data = (intptr_t) newWordCode;  // Set code array pointer
			newWordDictEntry->size = newWordCodeLen;
			newWordDictEntry->text = newWordText;

            // Check if this was a previously undefined word
			undefined_word_t * entry = undefSearch(newWordName, vocab);
			if(NULL != entry){
				// This word was on someone's wishlist!
				resolve_undefined_word(newWordName, newWordDictEntry, vocab);
			}else{
				printf("This was word was not previously referenced.\n");
			}

			// Detach
			newWordCode = NULL;
			newWordText = NULL;
			newWordDictEntry = NULL;

            PARSE_EXIT_STATE
            compiling = 0;
            break;
        }else if(!strcmp(tok,":")){
            printf("ERR: Definitions cannot be nested\n");
            assert(0);
        }
        // FALL-THROUGH
    case PARSE_NORMAL:
        // In this mode, we expect:
        // core words, user words, undef words, literal numbers, prints, strings.
        if((dict_entry = coreSearch(tok, vocab)) != NULL){
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
            }else if(!strcmp(tok,"VAR")){
                PARSE_ENTER_STATE(PARSE_VAR_S0);
            }else if(!strcmp(tok,"!")){
                PARSE_ENTER_STATE(PARSE_ASGN_S0);
            }else if(!strcmp(tok,"GROW")){
                PARSE_ENTER_STATE(PARSE_DICT_NEW);
            }else if(!strcmp(tok,"SHUT") || !strcmp(tok,"USE")){
                PARSE_ENTER_STATE(PARSE_DICT_EXIST);
            }
        }else if(!strcmp(tok,":")){
                compiling = 1;
                start_new_def();
                PARSE_ENTER_STATE(PARSE_COMPILE_S0);
        }else if((st = isPrint(tok)) != NULL){
            // Push the string info, and print it
            codeword_t * cw_push_literal = coreSearch("PUSHLIT", vocab);
            emit_cw(cw_push_literal);
			emit_cw((codeword_t *) st);
			emit_cw(cw_push_literal);
			emit_cw((codeword_t *) strlen(st));
			emit_cw(coreSearch("TOS", vocab));
        }else if((st = isString(tok)) != NULL){
            // Push the string info, but don't print it
            codeword_t * cw_push_literal = coreSearch("PUSHLIT", vocab);
            emit_cw(cw_push_literal);
			emit_cw((codeword_t *) st);
			emit_cw(cw_push_literal);
			emit_cw((codeword_t *) strlen(st));
        }else if(isNum(tok)){
            // It's a number
            emit_cw(coreSearch("PUSHLIT", vocab));
            emit_cw((codeword_t *) atol(tok));
        }else if((var_lookup = varSearch(tok, vocab)) != NULL){
                emit_cw(coreSearch("PUSHVAR", vocab));
                emit_cw((codeword_t *) var_lookup);
        }else if (isValidWordVarName(tok)){
            // It must be a user word, possibly undefined
            dict_entry = wordSearch(tok, vocab);
            if((dict_entry = wordSearch(tok, vocab)) != NULL){
                emit_cw(dict_entry);
            }else if((uword = undefSearch(tok, vocab)) != NULL){
                // Previously known undef word
                emit_cw(uword->placeholder);
            }else{
                // New undef word
                uword = create_undefined_word(tok, vocab);
                add_reference(uword, newWordDictEntry); // Make the undef word record point here
                emit_cw(uword->placeholder);
            }
        }else{
            printf("ERR: Unexpected %s\n",tok);
            assert(0);
        }
        break;
    case PARSE_IF_S0:
        // IF-/IF0/IF+ only need one branch target
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        PARSE_EXIT_STATE
        break;
    case PARSE_BR2_S0:
        // Expecting first of two branch targets
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        PARSE_CHANGE_STATE(PARSE_BR2_S1)
        break;
    case PARSE_BR2_S1:
        // Expecting second of two branch targets
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        PARSE_EXIT_STATE
        break;
    case PARSE_BRS_S0:
        // Expecting first of three branch targets
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        emit_cw(coreSearch("SKP1", vocab));
        PARSE_CHANGE_STATE(PARSE_BRS_S1)
        break;
    case PARSE_BRS_S1:
        // Expecting second of three branch targets
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        emit_cw(coreSearch("SKP1", vocab));
        PARSE_CHANGE_STATE(PARSE_BRS_S2)
        break;
    case PARSE_BRS_S2:
        // Expecting third of three branch targets
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
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
            assert(0);
        }
        break;
    case PARSE_BR_S1:
        // Expecting another branch condition (a literal constant) or an ELSE
        // This has to be an integer or an ELSE token
        if(isNum(tok)){
            // It's a number
            emit_cw(coreSearch("SKP2", vocab));
            emit_cw((codeword_t *) atol(tok));
            PARSE_CHANGE_STATE(PARSE_BR_S2)
        }else if(!strcmp("ELSE", tok)){
            // It's an ELSE
            // No emit
            PARSE_CHANGE_STATE(PARSE_BR_S3)
        }else{
            printf("ERR: Unexpected symbol %s in BR\n", tok);
            assert(0);
        }
        break;
    case PARSE_BR_S2:
        // Expecting a branch target
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        PARSE_CHANGE_STATE(PARSE_BR_S1)
        break;
    case PARSE_BR_S3:
        // Expecting one final branch target
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_cw(coreSearch("SKP1", vocab));
        emit_word_by_name(tok);
        PARSE_EXIT_STATE
        break;       
    case PARSE_DO_S0:
        // Expecting a word to loop
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        emit_cw(coreSearch("DO_LOOP", vocab));
        PARSE_EXIT_STATE
        break;
    case PARSE_RP_S0:
        // Expecting a word to loop
        // This might be a core word, a user word, or an undefined word (nothing else)
        emit_word_by_name(tok);
        emit_cw(coreSearch("RP_LOOP", vocab));
        PARSE_EXIT_STATE
        break;
    case PARSE_VAR_S0:
        // Expecting a variable name
        // This must not be a core word or existing/undefined user word/var, and must be alphanumeric
        // If we get it, PARSE_EXIT_STATE
        // Otherwise we need to error out of the parser
        if(!isValidWordVarName(tok) || coreSearch(tok, vocab) || growSearch(tok,vocab) || undefSearch(tok, vocab)){
            printf("ERR: Variable name %s not allowed\n",tok);
            assert(0);
        }
        // the VAR command was already emitted. We just emit a char * here.
        namebuf = malloc((strlen(tok)+1) * sizeof(char));
        strcpy(namebuf, tok);
        emit_cw((codeword_t *) namebuf);
        PARSE_EXIT_STATE
        break;
    case PARSE_ASGN_S0:
        // Expecting the name of a previously declared variable
        // Easily checked with varSearch
        if((var_lookup = varSearch(tok, vocab)) == NULL){
            printf("ERR: Unknown variable %s\n",tok);
            assert(0);
        }
        // The ! command was already emitted. We just emit a variable_t * here
        emit_cw((codeword_t *) var_lookup);
        PARSE_EXIT_STATE
        break;
    case PARSE_DICT_NEW:
        // Expecting a valid new dictionary name
        // Easily checked with isDict().
        if(!isDict(tok)){
            printf("ERR: Cannot name dictionary %s\n",tok);
            assert(0);
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
        if((sub = findDict(tok, vocab)) == NULL){
            printf("ERR: Unknown dictionary %s\n",tok);
            assert(0);
        }
        emit_cw((codeword_t *) sub);
        PARSE_EXIT_STATE
        break;
    default:
        printf("ERR: In unrecognized state %ld\n",parser_state);
        assert(0);
        break;
    }
    printf("Parse state %ld\n",parser_state);
    return 0;
}

int process_line(char * line){
    printf("*** Tokenizing line: ***\n>> %s", line);
    tokenizer_start_line(line);

    while(1){
        char * st = get_next_token();
        if(st == NULL){
            break;
        }else{
            printf("Parsing token %s\n",st);
            parse_tokens(st);
        }
    }
    printf("\n\n");
    return parser_state;
}