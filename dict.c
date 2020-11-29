/*	This file is part of libreDSSP.

	Copyright 2019 Alan Beadle

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

// Looks for defined variables
variable * varSearch(char * name, dict * vocab){
	variable * tempVar;
	if(name[0] == '\0') return NULL;
	if(vocab->var != NULL){
		tempVar = vocab->var;
		do{
			if(!strcmp(tempVar->name, name)){
				return tempVar;
			}
			tempVar = tempVar->next;
		}while(tempVar != NULL);
	}
	return NULL;
}

// Searches non-core dictionaries, returns word if it exists
word * wordSearch(char * name, dict * vocab){
	word * tempWord;
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

// Searches core dictionary, returns coreword if it exists
coreword * coreSearch(char * name, dict * vocab){
	coreword * tempCore = vocab->core;

	while(tempCore != NULL){
		if (!strcmp(tempCore->name, name)) return tempCore;
		tempCore = tempCore->next;
	}
	return NULL;
}

// Searches selected dictionary for word to redefine, or adds a new blank one of the specified name
word * wordDefine(char * name, dict * vocab){
	word * tempWord;
	word * lastWord;

	// If there is at least one word already
	if(vocab->grow->wordlist != NULL){
		if(vocab->grow == NULL){
			// This should have been caught in the parser
			printf("Fatal Error: We are defining a word but no dictionary is selected\n");
			assert(0);
		}else{
			if((vocab->grow->open) && (vocab->grow->wordlist != NULL)){
				tempWord = vocab->grow->wordlist;
				while(tempWord != NULL){
					if(!strcmp(tempWord->name, name)){
						printf("Found old definition of %s\n",name);
						return tempWord;
					}
					lastWord = tempWord;
					tempWord = tempWord->next;
				}
			}
		}
		printf("First definition of %s\n",name);
		// Didn't find the word
		// We will allocate it and link it to the dictionary
		lastWord->next = malloc(sizeof(word));
		tempWord = lastWord->next;
	}else{
		// This is the very first word in this dictionary
		printf("Dictionary begins with %s\n",name);
		vocab->grow->wordlist = malloc(sizeof(word));
		tempWord = vocab->grow->wordlist;
	}
	strcpy(tempWord->name, name);
	tempWord->next = NULL;
	// Regardless of initial state, now the word is named and present in the dictionary.
	// It's up to the caller to populate the word
	return tempWord;
}

void defCore(char * name, void (*func)(), dict * vocab){
	if(strlen(name) > CORE_NAME_LEN-1){
		printf("Fatal Error: Core word name %s exceeds %d characters\n",name,CORE_NAME_LEN-1);
		assert(0);
	}
	coreword * temp = NULL;
	if(vocab->core == NULL){
		temp = malloc(sizeof(coreword));
		strcpy(temp->name, name);
		temp->func = (*func);
		temp->next = NULL;
		vocab->core = temp;
		return;
	}else{
		temp = vocab->core;
		// Find last defined func
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = malloc(sizeof(coreword));
		strcpy(temp->next->name, name);
		temp->next->func = (*func);
		temp->next->next = NULL;
		return;
	}
}

subdict * newDict(dict * vocab, char * name){
	subdict * tempSub = vocab->sub;
	while(tempSub->next != NULL) tempSub = tempSub->next;
	tempSub->next = malloc(sizeof(subdict));
	tempSub = tempSub->next;
	tempSub->name = malloc(strlen(name)+1);
	strcpy(tempSub->name, name);
	tempSub->open = 1;
	tempSub->next = NULL;
	tempSub->wordlist = NULL;
	vocab->grow = tempSub;
	return tempSub;
}
