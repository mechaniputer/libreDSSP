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
#include <string.h>
#include <assert.h>

#include "dict.h"
#include "stack.h"
#include "cmdbuf.h"
#include "util.h"
#include "corewords.h"

// FIXME In order to unify the namespace of subdict words and subdict vars, we need this new function:
// Searches vocab->grow to ensure that a name is free in this namespace.
// Should be used whenever defining a new variable to ensure that it doessn't mask a function
// Should be used whenever defining a new word to ensure that it doesn't mask a variable.
// Return 0: Nothing found
// Return 1: Word found
// Return 2: Variable found
int growSearch(char * name, dict * vocab){
	return 0;
}

// Looks for defined variables
variable * varSearch(char * name, dict * vocab){
	variable * tempVar;
	if(name[0] == '\0') return NULL;
	/*if(vocab->var != NULL){
		tempVar = vocab->var;
		do{
			if(!strcmp(tempVar->name, name)){
				return tempVar;
			}
			tempVar = tempVar->next;
		}while(tempVar != NULL);
	}*/
	return NULL;
}

// Searches non-core dictionaries, returns codeword_t if it exists
codeword_t * wordSearch(char * name, dict * vocab){
	codeword_t * tempWord;
	subdict * tempSub;

	// Function must have a name greater than 1 char
	if(name[0] == '\0') return NULL;

	// Search subdicts
	tempSub = vocab->sub;
	while(tempSub != NULL){
		if((tempSub->open) && (tempSub->wordlist != NULL)){
			tempWord = tempSub->wordlist;
			while(tempWord != NULL){
				if(!strcmp(tempWord->name, name)){
					return tempWord;
				}
				tempWord = tempWord->next;
			}
		}
		tempSub = tempSub->next;
	}
	return NULL;
}

// Searches core dictionary, returns codeword_t if it exists
codeword_t * coreSearch(char * name, dict * vocab){
	codeword_t * tempCore = vocab->core;

	while(tempCore != NULL){
		if (!strcmp(tempCore->name, name)) return tempCore;
		tempCore = tempCore->next;
	}
	return NULL;
}

// Searches selected dictionary for word to redefine, or adds a new blank one of the specified name
codeword_t * wordDefine(char * name, dict * vocab){
	codeword_t * tempWord;

	if(vocab->grow == NULL){
		// TODO This should be handled gracefully (free resources, return NULL, print error).
		printf("Fatal Error: We are defining a word but no dictionary is selected\n");
		assert(0);
	}
	if(vocab->grow->open == 0){
		// TODO This should be handled gracefully (free resources, return NULL, print error).
		printf("Fatal Error: We are defining a word in a closed dictionary\n");
		assert(0);
	}
	// If there is at least one word already
	if(vocab->grow->wordlist != NULL){
		if(vocab->grow->wordlist != NULL){
			tempWord = vocab->grow->wordlist;
			while(tempWord != NULL){
				if(!strcmp(tempWord->name, name)){
					printf("Replacing old definition of %s in %s\n",name, vocab->grow->name);
					return tempWord;
				}
				tempWord = tempWord->next;
			}
		}
	}
	// Didn't find the word
	// We will allocate it and prepend it to the dictionary
	tempWord= malloc(sizeof(codeword_t));
	tempWord->next = vocab->grow->wordlist; // Might be NULL
	vocab->grow->wordlist = tempWord;

	tempWord->name = malloc((1+strlen(name))*sizeof(char));
	strcpy(tempWord->name, name);
	tempWord->xt = word_enter; // User words always call word_enter
	tempWord->data = 0;        // Will be set to code array pointer later
	tempWord->size = 0;        // Should correspond to number of elements in data[]
	tempWord->text = NULL;     // Will be set by parser
	tempWord->user = 1;        // Is user-defined
	// Regardless of initial state, now the word is named and present in the dictionary.
	// It's up to the caller to populate the word
	return tempWord;
}

void defCore(char * name, void (*func)(), dict * vocab){
	if(strlen(name) > CORE_NAME_LEN-1){
		printf("Fatal Error: Core word name %s exceeds %d characters\n",name,CORE_NAME_LEN-1);
		assert(0);
	}
	codeword_t * temp = malloc(sizeof(codeword_t));
	temp->xt = func;
	temp->data = 0;      // Core words don't use data field
	temp->size = 0;
	temp->name = name;
	temp->text = NULL;   // Core words don't have source text
	temp->next = NULL;
	temp->user = 0;      // Not user-defined

	if(vocab->core == NULL){
		vocab->core = temp;
		return;
	}else{
		codeword_t *traverse = vocab->core;
		while(traverse->next != NULL) traverse = traverse->next;
		traverse->next = temp;
		return;
	}
}

// Inserts a new subdictionary at the start of the linked list (as a stack)
subdict * newDict(dict * vocab, char * name){
	// Insert
	subdict * tempSub = malloc(sizeof(subdict));
	tempSub->next = vocab->sub;
	vocab->sub = tempSub;
	// Populate
	tempSub->name = malloc(strlen(name)+1);
	strcpy(tempSub->name, name);
	tempSub->open = 1;
	tempSub->wordlist = NULL;
	vocab->grow = tempSub;
	return tempSub;
}

subdict * findDict(dict * vocab, char * name){
	subdict * tempSub = vocab->sub;
	while(tempSub != NULL){
		if(!strcmp(tempSub->name, name)) return tempSub;
		tempSub = tempSub->next;
	}
	return NULL;
}

undefined_word_t* find_undefined_word(dict *vocab, char *name){
	printf("Looking for undefined word %s\n", name);
	undefined_word_t * temp = vocab->undefined;
	while(temp != NULL){
		//printf("Traversal sees %s\n",temp->name);
		if(!strcmp(temp->name, name)) return temp;
		temp = temp->next;
	}
	return NULL;
}

// Warning: Does not prevent adding duplicates
undefined_word_t* create_undefined_word(dict *vocab, char *name){
	printf("Creating undefined word %s\n",name);
	// Allocate and initialize
	undefined_word_t * newUndef = malloc(sizeof(undefined_word_t));
	newUndef->name = malloc(strlen(name));
	strcpy(newUndef->name, name);
	newUndef->references = malloc(4*sizeof(codeword_t *));
	newUndef->ref_count = 0;
	newUndef->ref_capacity = 4;

	// Create and attach a codeword_t that runs _undefined() core word
	newUndef->placeholder = malloc(sizeof(codeword_t));
	newUndef->placeholder->xt = _undefined;
	newUndef->placeholder->name = newUndef->name; // Recycle same name buffer allocated above
	newUndef->placeholder->data = 0;
	newUndef->placeholder->text = NULL;
	newUndef->placeholder->next = NULL;
	newUndef->placeholder->user = 0; // Not user-defined word

	// Insert and return
	newUndef->next = vocab->undefined;
	vocab->undefined = newUndef;
	return newUndef;
}

void add_reference(undefined_word_t *undef, codeword_t *ref){
	printf("In add_reference()\n");
	if(undef->ref_count == undef->ref_capacity){
		undef->ref_capacity += 4;
		undef->references = realloc(undef->references, undef->ref_capacity * sizeof(codeword_t *));
	}
	undef->references[undef->ref_count] = ref;
	undef->ref_count += 1;
}

// WARNING: If the words that referenced this undefined word no longer exist, it can segfault.
// Adds missing refs to formerly undefined word, and then removed the undef record.
void resolve_undefined_word(dict *vocab, char *name, codeword_t *def){
	printf("Resolving previously undefined %s\n", name);
	// First we need to find the word in the linked list.
	// We need to keep a pointer to the previous word in the list.
	undefined_word_t * curr = vocab->undefined;
	undefined_word_t * prev = NULL;
	while(curr != NULL){
		if(!strcmp(curr->name, name)) break;
		prev = curr;
		curr = curr->next;
	}

	// If curr is NULL, the word is not in the undef list after all.
	if(curr == NULL){
		printf("Error: Word %s is not in the undefined words list.\n", name);
		exit(0);
	}

	// Splice out curr.
	// If prev is NULL then we are dealing with the head of the list.
	if(prev == NULL){
		vocab->undefined = curr->next;
	}else{
		prev->next = curr->next;
	}

	// Now we have the undef record curr, and the new word, def.
	// We need to find all occurrences of the undefined word (iterate curr->references)
	// Each one points to the codeword_t of a word that needed this new word.
	for(int i=0; i<curr->ref_count ; i++){
		// i denotes just one of our references
		int num_words = curr->references[i]->size;
		codeword_t ** dependent_array = (codeword_t **) curr->references[i]->data;
		for(int j=0; j<num_words; j++){
			if(!strcmp(name, dependent_array[j]->name)){
				// Found a match
				dependent_array[j] = def;
			}
		}
	}

	// TODO frees
	return;
}