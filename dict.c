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

extern dict * vocab;

// Global singleton codewords
extern codeword_t cw_var_undef_assign;
extern codeword_t cw_var_generic_assign;
extern codeword_t cw_var_undef_addrof;
extern codeword_t cw_var_generic_addrof;
extern codeword_t cw_var_undef_set0;
extern codeword_t cw_var_generic_set0;
extern codeword_t cw_var_undef_set1;
extern codeword_t cw_var_generic_set1;

// Searches vocab->grow to ensure that a name is free in this namespace.
// Should be used whenever defining a new variable to ensure that it doesn't mask a function
// Should be used whenever defining a new word to ensure that it doesn't mask a variable.
// Return 0: Nothing found
// Return 1: Word found
// Return 2: Variable found
// Return 3: Undefined object found
int collisionSearch(char * name){
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
	if(NULL != undefSearch(name)){
		return 3;
	}
	return 0;
}

// Looks for defined variables
// TODO add a way for the user to configure the subdict search order
variable_t * varSearch(char * name){
	variable_t * tempVar;
	subdict * tempSub;

	// Variable must have a name greater than 1 char
	if(name[0] == '\0') return NULL;

	// Search subdicts
	tempSub = vocab->sub;
	while(tempSub != NULL){
		//printf("Searching subdict %s for vars\n",tempSub->name);
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
codeword_t * wordSearch(char * name){
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
codeword_t * coreSearch(char * name){
	codeword_t * tempCore = vocab->core;

	while(tempCore != NULL){
		if (!strcmp(tempCore->name, name)) return tempCore;
		tempCore = tempCore->next;
	}
	return NULL;
}

// Searches selected dictionary for word to redefine, or adds a new blank one of the specified name
codeword_t * wordDefine(char * name){
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

void defCore(char * name, void (*func)()){
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
subdict * newDict(char * name){
	// Insert
	subdict * tempSub = malloc(sizeof(subdict));
	tempSub->next = vocab->sub;
	vocab->sub = tempSub;
	// Populate
	tempSub->name = malloc(strlen(name)+1);
	strcpy(tempSub->name, name);
	tempSub->open = 1;
	tempSub->wordlist = NULL;
	tempSub->varlist = NULL;
	vocab->grow = tempSub;
	return tempSub;
}

subdict * findDict(char * name){
	subdict * tempSub = vocab->sub;
	while(tempSub != NULL){
		if(!strcmp(tempSub->name, name)) return tempSub;
		tempSub = tempSub->next;
	}
	return NULL;
}

undefined_ref_t * undefSearch(char *name){
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
undefined_ref_t * create_undefined_ref(char *name){
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

	// Insert and return
	newUndef->next = vocab->undefined;
	vocab->undefined = newUndef;
	return newUndef;
}

// Scans all ref pointers.
// If any point to something other than uref->ref_placeholder, return 1.
// Else return 0
int referenced_as_var(char * name){
	undefined_ref_t * uref = undefSearch(name);
	if(!uref) return 0;

	codeword_t * placeholder = uref->ref_placeholder;
	int ref_count = uref->ref_count;
	codeword_t *** refs = uref->references;

	for(int i=0; i<ref_count ; i++){
		if(placeholder != *(refs[i])){
			// This is a var-only refernce
			printf("This name can only be a variable\n");
			return 1;
		}
	}
	printf("This name is not referenced as a variable\n");
	return 0;
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

// Adds missing refs to formerly undefined word, and then removed the undef record.
// Warning:
// This function assumes that if we are patching word refs, none of them are a var op.
// Code calling this function must be certain that outstanding refs are safe to patch.
void resolve_undefined_ref(char *name, codeword_t *def, int isVar, variable_t * var){
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

	// TODO ensure that if isVar is False, every ref is to curr_ref_placeholder (otherwise crash)
	// Patch each ref location
	for(int i=0; i<curr->ref_count ; i++){
		codeword_t ** patch_target = curr->references[i];
		if(*patch_target == curr->ref_placeholder){
			if(0 == isVar){
				// It's a word
				*patch_target = def;
			}else{
				// It's a var
				*patch_target = &var->cw_pushVar; // _var8_ref
			}
		}else if(isVar && (*patch_target == &cw_var_undef_assign)){
			// [&cw_var_generic_assign] [&variable_t var]
			*patch_target++ = &cw_var_generic_assign;
			*patch_target = (codeword_t *) var;
		}else if(isVar && (*patch_target == &cw_var_undef_addrof)){
			// [&cw_var_generic_addrof] [&variable_t var]
			*patch_target++ = &cw_var_generic_addrof;
			*patch_target = (codeword_t *) var;
		}else if(isVar && (*patch_target == &cw_var_undef_set0)){
			// [&cw_var_generic_set0] [&variable_t var]
			*patch_target++ = &cw_var_generic_set0;
			*patch_target = (codeword_t *) var;
		}else if(isVar && (*patch_target == &cw_var_undef_set1)){
			// [&cw_var_generic_set1] [&variable_t var]
			*patch_target++ = &cw_var_generic_set1;
			*patch_target = (codeword_t *) var;
		}else{
			printf("ERR: Unexpected value at patch target\n");
			printf("Name: %s\n", (*patch_target)->name);
			exit(-1);
		}
	}

	free(curr->name);
	free(curr->ref_placeholder);
	free(curr->references);
	free(curr);
	return;
}


// Scans the entire dictionary (of words, not vars) for refs to the specified undef (word or var)
// Changes them to point to the undef's placeholder.
// Any word in any subdict might refernce the word we are deleting.
void patch_to_undef(undefined_ref_t * uref, codeword_t * old_cw){

	subdict * tempSub;

	// Search all subdicts
	tempSub = vocab->sub;
	while(tempSub != NULL){
		// Search every word in the current subdict
		codeword_t * curr_word = tempSub->wordlist;
		while(curr_word != NULL){
			printf("Patching body of word %s\n",curr_word->name);
			int loop_end_index = -1; // This trick is also used in print_codewords() and deleteName()
			codeword_t ** array = (codeword_t **) (curr_word->data);
			print_codewords(array);
			int patch_index = 0;

			while(array[patch_index] != NULL || (patch_index < loop_end_index)){ // Depends on NULL sentinel

				// Skip branch literals
				if((patch_index <= loop_end_index) && ((loop_end_index-patch_index) % 3 == 0)) patch_index++;

				// We need to distinguish whether this is a plain word ref, a plain var ref, or a 2-cell var ref
				if((codeword_t *) array[patch_index] == old_cw){
					if(!strcmp(array[patch_index]->name, uref->name)){
						// Word calls become a plain ref
						//printf("Patching plain word ref\n");
						add_reference(uref, &array[patch_index]);
						array[patch_index] = uref->ref_placeholder;
					}
				}else if(array[patch_index]->xt == _var8_ref){
					if(!strcmp(array[patch_index]->name, uref->name)){
						// If it's a plain var ref:
						//printf("Patching plain var ref\n");
						add_reference(uref, &array[patch_index]);
						array[patch_index] = uref->ref_placeholder;
					}
				}else if(array[patch_index] == &cw_var_generic_assign){
					if(!strcmp( ((variable_t *)(array[patch_index+1]))->name , uref->name)){
						//printf("Patching var assign ref\n");
						add_reference(uref, &array[patch_index]);
						array[patch_index++] = &cw_var_undef_assign;
						array[patch_index] = (codeword_t *) uref->name;
					}
				}else if(array[patch_index] == &cw_var_generic_addrof){
					if(!strcmp( ((variable_t *)(array[patch_index+1]))->name , uref->name)){
						//printf("Patching var addrof ref\n");
						add_reference(uref, &array[patch_index]);
						array[patch_index++] = &cw_var_undef_addrof;
						array[patch_index] = (codeword_t *) uref->name;
					}
				}else if(array[patch_index] == &cw_var_generic_set0){
					if(!strcmp( ((variable_t *)(array[patch_index+1]))->name , uref->name)){
						//printf("Patching var !0 ref\n");
						add_reference(uref, &array[patch_index]);
						array[patch_index++] = &cw_var_undef_set0;
						array[patch_index] = (codeword_t *) uref->name;
					}
				}else if(array[patch_index] == &cw_var_generic_set1){
					if(!strcmp( ((variable_t *)(array[patch_index+1]))->name , uref->name)){
						//printf("Patching var !1 ref\n");
						add_reference(uref, &array[patch_index]);
						array[patch_index++] = &cw_var_undef_set1;
						array[patch_index] = (codeword_t *) uref->name;
					}
				}else if(array[patch_index]->xt == pushLiteral || array[patch_index]->xt == declare_var8 || array[patch_index]->xt == growSub || array[patch_index]->xt == growSub || array[patch_index]->xt == shutSub){
					// For some additional words we need to skip two cells for safety
					patch_index += 1;
				}else if(!strcmp(array[patch_index]->name, "BR")){
					// Example code:    BR 0 FOO 1 BAR 2 BAZ ELSE ERG
					// Compiled result: BR 0 FOO SKP2 1 BAR SKP2 2 BAZ SKP1 ERG
					for(loop_end_index = patch_index+3; ; loop_end_index+=3){
						if(array[loop_end_index]->xt != skip2) break;
					}
					// loop_end_index now points to ERG in the example above (position 9)
					// We want it to point to the last literal (position 7):
					loop_end_index -= 2;
					//printf("Loop end index is %d\n",loop_end_index);
				}
				patch_index += 1; // Next codeword_t *
			}

			curr_word = curr_word->next;
		}
		tempSub = tempSub->next;
	}
}

// Finds an undef entry in the list and removes/frees it
void delete_undef_ref(undefined_ref_t * uref){
	undefined_ref_t * curr = vocab->undefined;
	undefined_ref_t * prev = NULL;
	while(curr != NULL){
		if(curr == uref) break;
		prev = curr;
		curr = curr->next;
	}

	if(curr == NULL){
		printf("ERR: Undef ref not found in delete_undef_ref\n");
		exit(-1);
	}

	if(prev == NULL){
		vocab->undefined = curr->next;
	}else{
		prev->next = curr->next;
	}

	free(curr->name);
	free(curr->ref_placeholder);
	free(curr->references);
	free(curr);
	return;
}

// Takes an array of codewords and untracks any undef refs made therein
// FIXME there's a lot of repeated logic here. Needs improvement.
void untrack_undef_refs(codeword_t ** array){
char * temp_name;
	int ind = 0;
	int loop_end_index = -1; // This trick is also used in print_codewords() and patch_to_undef()
	while(array[ind] != NULL || (ind < loop_end_index)){ // Relies on NULL sentinel

		// Skip branch literals
		if((ind <= loop_end_index) && ((loop_end_index-ind) % 3 == 0)) ind++;

		printf("Scanning deletion target: Checking whether %s is an undef ref\n", array[ind]->name);
		// If we encounter an undef ref, we need to un-track it to prevent a later segfault

		if(array[ind]->xt == _undefined_ref){
			temp_name = array[ind]->name;
			// Reference to undefined word
			printf("This word references undefined word %s!\n",temp_name);
			// We need to retrieve the undef record, remove the ref, and if the undef word hits 0 refs, get rid of it.
			undefined_ref_t * uref = undefSearch(temp_name);
			if(uref == NULL){
				printf("ERR: Reference to nonexistent undefined word %s\n",temp_name);
				exit(-1);
			}

			// We have the undef record. Now we search for a pointer match.
			for(int j=0; j<uref->ref_count; j++){
				if(uref->references[j] == &array[ind]){
					// We found the ref to remove. We swap it with the last ref and decrement the count.
					uref->references[j] = uref->references[uref->ref_count - 1];
					uref->ref_count -= 1;
					break;
				}
			}

			// If this undef word has no more refs, we can remove it from the undef table and free it.
			if(uref->ref_count == 0){
				printf("Undef word %s has no more refs, removing from undef table\n",uref->name);
				// Call deletion helper to splice out the undef record and free it
				delete_undef_ref(uref);
			}
			ind += 1;
		}else if(array[ind]->xt == _var_undef_assign){
			// Reference to undefined variable
			// Note: This will later be a list of like 10 functions to check for... Probably need a helper function.
			temp_name = (char *) array[ind+1];
			printf("This word references undefined variable %s!\n",temp_name);

			// We need to retrieve the undef record, remove the ref, and if the undef word hits 0 refs, get rid of it.
			undefined_ref_t * uref = undefSearch(temp_name);
			if(uref == NULL){
				printf("ERR: Reference to nonexistent undefined word %s\n",temp_name);
				exit(-1);
			}

			// We have the undef record. Now we search for a pointer match.
			for(int j=0; j<uref->ref_count; j++){
				if(uref->references[j] == &array[ind]){
					// We found the ref to remove. We swap it with the last ref and decrement the count.
					uref->references[j] = uref->references[uref->ref_count - 1];
					uref->ref_count -= 1;
					break;
				}
			}

			// If this undef word has no more refs, we can remove it from the undef table and free it.
			if(uref->ref_count == 0){
				printf("Undef word %s has no more refs, removing from undef table\n",uref->name);
				// Call deletion helper to splice out the undef record and free it
				delete_undef_ref(uref);
			}
			// Important: skip 2 cells since var-specific ops are 2 cells.
			ind += 2;

		}else if(array[ind]->xt == pushLiteral || array[ind]->xt == declare_var8 || array[ind]->xt == growSub || array[ind]->xt == growSub || array[ind]->xt == shutSub){
			// For some words we need to skip two cells for safety
			ind += 1;
		}else if(!strcmp(array[ind]->name, "BR")){
			// Example code:    BR 0 FOO 1 BAR 2 BAZ ELSE ERG
			// Compiled result: BR 0 FOO SKP2 1 BAR SKP2 2 BAZ SKP1 ERG
			for(loop_end_index = ind+3; ; loop_end_index+=3){
				if(array[loop_end_index]->xt != skip2) break;
			}
			// loop_end_index now points to ERG in the example above (position 9)
			// We want it to point to the last literal (position 7):
			loop_end_index -= 2;
			//printf("Loop end index is %d\n",loop_end_index);
		}
		ind += 1; // Next codeword_t *
	}
}

void deleteName(char * name){
	// Note: deletion is constrained to the current subdict
	// Because no name collisions are allowed within a subdict, this will only delete one object.
	// It might be a word or a variable.

	// Check for a word to delete.
	codeword_t * curr_word = vocab->grow->wordlist;
	codeword_t * prev_word = NULL;
	while(curr_word != NULL){
		if(!strcmp(curr_word->name, name)){
			// If found, splice it out.
			printf("Found word \"%s\" to delete\n",name);
			if(prev_word == NULL){
				// It was literally the first word in the list
				vocab->grow->wordlist = curr_word->next;
			}else{
				prev_word->next = curr_word->next;
			}
			break;
		}
		prev_word = curr_word;
		curr_word = curr_word->next;
	}

	// Delete the word and return
	if(curr_word != NULL){
		printf("DEL %s:%s\n",vocab->grow->name, curr_word->name);
		// First, scan the body of this word to see if it contains any undef refs
		codeword_t ** array = (codeword_t**) curr_word->data;

		// If it does, untrack them (this can result in undef words becoming totally un-unreferenced and deleted)
		untrack_undef_refs(array);

		// Create a new undef ref for this entity.
		undefined_ref_t * uref = create_undefined_ref(name);

		// Scan all user words in vocab->grow for references. Add refs and patch them to the appropriate undef placeholder.
		patch_to_undef(uref, curr_word);

		// Free the old word
		free(curr_word->name);
		free(curr_word->text);
		free((void *) curr_word->data);
		free(curr_word);

		// If uref shows no refs, we can remove it immediately.
		if(uref->ref_count == 0){
			//printf("Undef word %s has no refs, removing from undef table\n",uref->name);
			delete_undef_ref(uref);
		}
		return;
	}

	// Check for a var to delete.
	variable_t * curr_var = vocab->grow->varlist;
	variable_t * prev_var = NULL;
	while(curr_var != NULL){
		if(!strcmp(curr_var->name, name)){
			// If found, splice it out.
			//printf("Found var \"%s\" to delete\n",name);
			if(prev_var == NULL){
				// It was literally the first var in the list
				vocab->grow->varlist = curr_var->next;
			}else{
				prev_var->next = curr_var->next;
			}
			break;
		}
		prev_var = curr_var;
		curr_var = curr_var->next;
	}

	// Delete the var and return
	if(curr_var != NULL){
		printf("DEL %s:%s\n",vocab->grow->name, curr_var->name);
		// Create a new undef ref for this entity.
		undefined_ref_t * uref = create_undefined_ref(name);

		// Scan all user words in vocab->grow for references. Add refs and patch them to the appropriate undef placeholder.
		patch_to_undef(uref, /*old_cw*/ NULL);
		// Free the old word
		free(curr_var->name);
		free(curr_var);

		// If uref shows no refs, we can remove it immediately.
		if(uref->ref_count == 0){
			//printf("Undef var %s has no refs, removing from undef table\n",uref->name);
			delete_undef_ref(uref);
		}
		return;
	}

}
