/*	This file is part of libreDSSP.

	Copyright 2016 Alan Beadle

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
#include "elem.h"
#include "stack.h"
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
			do{
				if(!strcmp(tempWord->name, name)){
					return tempWord;
				}
				tempWord = tempWord->next;
			}while(tempWord != NULL);
		}
		tempSub = tempSub->next;
	}

	return NULL;
}

// Searches non-core dictionaries, returns 1 if it exists
int coreSearch(char * name, dict * vocab){
	coreword * tempCore = vocab->core;

	while(tempCore != NULL){
		if (!strcmp(tempCore->name, name)) return 1;
		tempCore = tempCore->next;
	}
	return 0;
}

word * newWord(subdict * dict){
	assert(dict != NULL);

	word * temp; // Lets us find the next empty spot

	if(dict->wordlist == NULL){ // First word in dictionary
		dict->wordlist = malloc(sizeof(word));
		temp = dict->wordlist;
	} else { // Not first word in dictionary
		temp = dict->wordlist;
		while(temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = malloc(sizeof(word));
		temp = temp->next;
	}
	// temp should now point to fresh word
	return temp;
}

// Attempts to define a new word
void defWord(cmdstack * cmdstack, dict * vocab){
	word * temp;
	assert(vocab != NULL);
	assert(vocab->sub != NULL);

	if(vocab->grow == NULL) vocab->grow = vocab->sub; // Grow first dict by default

	// Skip over ':'
	if(cmdstack->top >= 0){
		cmdPop(cmdstack);
	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
	}

	// Assign function name
	if(cmdstack->top >= 0){

		// See if it is a core word
		if(coreSearch(cmdTop(cmdstack), vocab)){
			fprintf(stderr,"ERROR: %s is in core dictionary\n",cmdTop(cmdstack));
			return;
		}

		// See if we already have this word, if we do then redefine
		temp = wordSearch(cmdTop(cmdstack), vocab);

		if(temp == NULL){
			// Append the new word
			temp = newWord(vocab->grow);
			strcpy(temp->name, cmdPop(cmdstack));
		}else{
			// Wipe old definition
			strcpy(temp->definition, "");
			cmdPop(cmdstack);
		}

	}else fprintf(stderr,"ERROR: Incomplete definition\n");

	while(strcmp(";", cmdTop(cmdstack))){
		strcat(temp->definition, cmdPop(cmdstack));
		strcat(temp->definition, " ");

		if(cmdstack->top < 0){
			fprintf(stderr,"ERROR: Incomplete definition\n");
			return;
		}
	}
	// Get rid of ";"
	cmdPop(cmdstack);

	// Successful definition, print name
	printf("%s\n",temp->name);
	return;
}

void defCore(char * name, void (*func)(stack *, cmdstack *, dict*), dict * vocab){
	coreword * temp = vocab->core;
	if(vocab->core == NULL){
		temp = malloc(sizeof(coreword));
		strcpy(temp->name, name);
		temp->func = (*func);
		temp->next = NULL;
	}
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

subdict * newDict(dict * vocab, char * name){
	subdict * tempSub = vocab->sub;
	while(tempSub->next != NULL) tempSub = tempSub->next;
	tempSub->next = malloc(sizeof(subdict));
	tempSub = tempSub->next;
	tempSub->name = name;
	tempSub->open = 1;
	tempSub->next = NULL;
	vocab->grow = tempSub;
	return tempSub;
}
