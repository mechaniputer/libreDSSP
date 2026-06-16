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


extern codeword_t global_cw_assign_to_word_err ;

// Searches vocab->grow to ensure that a name is free in this namespace.
// Should be used whenever defining a new variable to ensure that it doesn't mask a function
// Should be used whenever defining a new word to ensure that it doesn't mask a variable.
// Return 0: Nothing found
// Return 1: Word found
// Return 2: Variable found
// Return 3: Undefined object found
int collisionSearch(char * name, dict * vocab){
	variable_t * tempVar;
	codeword_t * tempWord;

	assert(vocab->grow != NULL);
	assert(vocab->grow->open);

	subdict * growsub = vocab->grow;

	//printf("growsub() searches dictionary %s\n",growsub->name);
	// Search for word name collisions
	tempWord = growsub->wordlist;
	while(tempWord != NULL){
		if(!strcmp(tempWord->name, name)){
			return 1;
		}
		tempWord = tempWord->next;
	}

	// Search for var name collisions
	tempVar = growsub->varlist;
	while(tempVar != NULL){
		//printf("growsub() sees %s\n",tempVar->name);
		if(!strcmp(tempVar->name, name)){
			return 2;
		}
		tempVar = tempVar->next;
	}

	// Last we check for undefined words with that name
	if(NULL != undefSearch(name, vocab)){
		return 3;
	}
	return 0;
}

// Looks for defined variables
// TODO add a way for the user to configure the subdict search order
variable_t * varSearch(char * name, dict * vocab){
	variable_t * tempVar;
	subdict * tempSub;

	// Variable must have a name greater than 1 char
	if(name[0] == '\0') return NULL;

	// Search subdicts
	tempSub = vocab->sub;
	while(tempSub != NULL){
		if((tempSub->open) && (tempSub->varlist != NULL)){
			tempVar = tempSub->varlist;
			while(tempVar != NULL){
				if(!strcmp(tempVar->name, name)){
					return tempVar;
				}
				tempVar = tempVar->next;
			}
		}
		tempSub = tempSub->next;
	}
	return NULL;
}

// Searches non-core dictionaries, returns codeword_t if it exists
// TODO add a way for the user to configure the subdict search order
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
		printf("Error: We are defining a word but no dictionary is selected\n");
		abortExecution();
	}
	if(vocab->grow->open == 0){
		printf("Error: We are defining a word in a closed dictionary\n");
		abortExecution();
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
	tempWord->data = 0;     // Will be set to code array pointer later
	tempWord->size = 0;     // Should correspond to number of elements in data[]
	tempWord->text = NULL;  // Will be set by parser
	// Regardless of initial state, now the word is named and present in the dictionary.
	// It's up to the caller to populate the word
	return tempWord;
}

void defCore(char * name, void (*func)(), dict * vocab){
	if(strlen(name) > CORE_NAME_LEN-1){
		printf("Fatal Error: Core word name %s exceeds %d characters\n",name,CORE_NAME_LEN-1);
		exit(-1); // This is not an error we should recover from as it indicates a problem in libreDSSP itself
	}
	codeword_t * temp = malloc(sizeof(codeword_t));
	temp->xt = func;
	temp->data = 0;      // Core words don't use data field
	temp->size = 0;
	temp->name = name;
	temp->text = NULL;   // Core words don't have source text
	temp->next = NULL;

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
subdict * newDict(char * name, dict * vocab){
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

subdict * findDict(char * name, dict * vocab){
	subdict * tempSub = vocab->sub;
	while(tempSub != NULL){
		if(!strcmp(tempSub->name, name)) return tempSub;
		tempSub = tempSub->next;
	}
	return NULL;
}

undefined_ref_t * undefSearch(char *name, dict *vocab){
	//printf("Looking for undefined word %s\n", name);
	undefined_ref_t * temp = vocab->undefined;
	while(temp != NULL){
		//printf("Traversal sees %s\n",temp->name);
		if(!strcmp(temp->name, name)) return temp;
		temp = temp->next;
	}
	return NULL;
}

// Warning: Does not prevent adding duplicates
undefined_ref_t * create_undefined_ref(char *name, dict *vocab){
	//printf("Creating undefined word %s\n",name);
	// Allocate and initialize
	undefined_ref_t * newUndef = malloc(sizeof(undefined_ref_t));
	newUndef->name = malloc(1+strlen(name));
	strcpy(newUndef->name, name);
	newUndef->references = malloc(4*sizeof(codeword_t *));
	newUndef->ref_count = 0;
	newUndef->ref_capacity = 4;

	// Create and attach a codeword_t that runs _undefined_ref() core word
	newUndef->ref_placeholder = malloc(sizeof(codeword_t));
	newUndef->ref_placeholder->xt = _undefined_ref;
	newUndef->ref_placeholder->name = newUndef->name; // Recycle same name buffer allocated above
	newUndef->ref_placeholder->data = 0;
	newUndef->ref_placeholder->text = NULL;
	newUndef->ref_placeholder->next = NULL;

	// Same thing but for undefAssignVar()
	newUndef->assign_placeholder = malloc(sizeof(codeword_t));
	newUndef->assign_placeholder->xt = _undefined_assign;
	newUndef->assign_placeholder->name = newUndef->name; // Recycle same name buffer allocated above
	newUndef->assign_placeholder->data = 0;
	newUndef->assign_placeholder->text = NULL;
	newUndef->assign_placeholder->next = NULL;

	// Insert and return
	newUndef->next = vocab->undefined;
	vocab->undefined = newUndef;
	return newUndef;
}

// Previously each ref was to the codeword_t "header" for the word containing the ref.
// Now it's a direct pointer to the ref.
void add_reference(undefined_ref_t *undef, codeword_t **ref){
	//printf("In add_reference()\n");
	if(undef->ref_count == undef->ref_capacity){
		undef->ref_capacity += 4;
		undef->references = realloc(undef->references, undef->ref_capacity * sizeof(codeword_t *));
	}
	undef->references[undef->ref_count] = ref;
	undef->ref_count += 1;
}

// WARNING: If the words that referenced this undefined word no longer exist, it can segfault.
//          Currently, deleting words is not supported, but later it will be.
// Adds missing refs to formerly undefined word, and then removed the undef record.
void resolve_undefined_ref(char *name, codeword_t *def, int isVar, variable_t * var, dict *vocab){
	//printf("Resolving previously undefined %s\n", name);

	// Check the more error-prone params
	if(isVar) assert(var != NULL);
	if(!isVar) assert(def != NULL);

	// First we need to find the word in the linked list.
	// We need to keep a pointer to the previous word in the list.
	undefined_ref_t * curr = vocab->undefined;
	undefined_ref_t * prev = NULL;
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

	// Patch each ref location
	for(int i=0; i<curr->ref_count ; i++){
		codeword_t ** patch_target = curr->references[i];
		if(*patch_target == curr->ref_placeholder){
			if(0 == isVar){
				// It's a word
				*patch_target = def;
			}else{
				// It's a var
				*patch_target = &var->cw[0]; // pushVar
			}
		}else if(*patch_target == curr->assign_placeholder){
			if(0 == isVar){
				// Can't assign values to words
				*patch_target = &global_cw_assign_to_word_err;
			}else{
				// It's a var
				*patch_target = &var->cw[1]; // assignVar
			}
		}else{
			printf("ERR: Unexpected value at patch target\n");
			printf("Name: %s\n", (*patch_target)->name);
			exit(-1);
		}
	}

	free(curr->name);
	free(curr->ref_placeholder);
	free(curr->assign_placeholder);
	free(curr->references);
	free(curr);
	return;
}