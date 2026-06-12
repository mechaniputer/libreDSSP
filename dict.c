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

// Searches vocab->grow to ensure that a name is free in this namespace.
// Should be used whenever defining a new variable to ensure that it doesn't mask a function
// Should be used whenever defining a new word to ensure that it doesn't mask a variable.
// Return 0: Nothing found
// Return 1: Word found
// Return 2: Variable found
// Return 3: Undefined word found
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
	tempWord->userDef = 0;     // Will be set to code array pointer later
	tempWord->size = 0;        // Should correspond to number of elements in userDef[]
	tempWord->text = NULL;     // Will be set by parser
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
	temp->userDef = 0;      // Core words don't use userDef field
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

undefined_word_t* undefSearch(char *name, dict *vocab){
	//printf("Looking for undefined word %s\n", name);
	undefined_word_t * temp = vocab->undefined;
	while(temp != NULL){
		//printf("Traversal sees %s\n",temp->name);
		if(!strcmp(temp->name, name)) return temp;
		temp = temp->next;
	}
	return NULL;
}

// Warning: Does not prevent adding duplicates
undefined_word_t* create_undefined_word(char *name, dict *vocab){
	//printf("Creating undefined word %s\n",name);
	// Allocate and initialize
	undefined_word_t * newUndef = malloc(sizeof(undefined_word_t));
	newUndef->name = malloc(1+strlen(name));
	strcpy(newUndef->name, name);
	newUndef->references = malloc(4*sizeof(codeword_t *));
	newUndef->ref_count = 0;
	newUndef->ref_capacity = 4;

	// Create and attach a codeword_t that runs _undefined() core word
	newUndef->placeholder = malloc(sizeof(codeword_t));
	newUndef->placeholder->xt = _undefined;
	newUndef->placeholder->name = newUndef->name; // Recycle same name buffer allocated above
	newUndef->placeholder->userDef = 0;
	newUndef->placeholder->text = NULL;
	newUndef->placeholder->next = NULL;

	// Insert and return
	newUndef->next = vocab->undefined;
	vocab->undefined = newUndef;
	return newUndef;
}

void add_reference(undefined_word_t *undef, codeword_t *ref){
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
void resolve_undefined_word(char *name, codeword_t *def, dict *vocab){
	//printf("Resolving previously undefined %s\n", name);
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
		//printf("num_words is %d\n", num_words);
		codeword_t ** dependent_array = (codeword_t **) curr->references[i]->userDef;
		//print_codewords(dependent_array);

		int loop_end_index=-1;

		for(int j=0; j<num_words; j++){
			//printf("resolve_undefined_word() looking for name %s in array index %d\n",name,j);
			//printf("See name %s \n", dependent_array[j]->name);
			if(!strcmp(name, dependent_array[j]->name)){
				// Found a match
				dependent_array[j] = def;
			}
			if(!strcmp(dependent_array[j]->name, "PUSHLIT") || !strcmp(dependent_array[j]->name, "PUSHVAR") || !strcmp(dependent_array[j]->name, "!")){
				//printf("Skipping literal\n");
				// Skip the literal
				j+=1;
				continue;
			}

			//printf("array index is now %d\n",j);
			// Example definition: BR 0 FOO SKP2 1 BAR SKP2 2 BAZ SKP1 ERG
			if(!strcmp(dependent_array[j]->name, "BR")){
				//printf("It's a BR\n");
				// Find where the SKP1 is
				for(loop_end_index = j+3; ; loop_end_index+=3){
					//printf("loop_end_index=%d\n",loop_end_index);
					//printf("See function %s\n",dependent_array[loop_end_index]->name);
					if(dependent_array[loop_end_index]->xt == skip1) break;
				}
				// Should be the index of the branch targer before SKP1
				loop_end_index -= 1;
				//printf("Loop end index is %d\n",loop_end_index);
				// Remember that the loop adds 1 also. By adding one here we land on BR+2
				j += 1;
			}else if(j < loop_end_index){
				// Remember that the loop adds 1 also. By adding 2 here we take triple steps.
				j+=2;
			}
		}
	}

	free(curr->name);
	free(curr->placeholder);
	free(curr->references);
	free(curr);
	return;
}