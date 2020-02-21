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

// TODO Rewrite for new word definition style
word * newWord(subdict * dict){
	assert(dict != NULL);

/*
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
	temp->array = malloc(15*sizeof(command));
	temp->length = -1;
	temp->capacity = 15;
	temp->next = NULL;
	// temp should now point to fresh word
	return temp;
*/
	return malloc(sizeof(word));
}

// Attempts to define a new word
// TODO rewrite to produce list of pointers and store it the Forth way
void defWord(cmdbuffer * cmdbuf, dict * vocab){
/*	word * temp;
	assert(vocab != NULL);
	assert(vocab->sub != NULL);

	// Shouldn't just pick one that's open since we don't want to pollute a random dictionary
	if(vocab->grow == NULL){
		fprintf(stderr,"ERROR: Target dictionary not designated\n");
		cmdClear(cmdbuf);
		return;
	}

	// Skip over ':'
	if(cmdbuf->size >= 2){
		cmdDrop(cmdbuf);
	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
		cmdClear(cmdbuf);
		return;
	}

	// Assign function name
	if(cmdbuf->top >= 1){

		// See if it is a core word
		if(coreSearch(cmdTop(cmdbuf)->text, vocab)){
			fprintf(stderr,"ERROR: %s is in core dictionary\n",cmdTop(cmdbuf)->text);
			cmdClear(cmdbuf);
			return;
		}

		// See if we already have this word, if we do then redefine
		temp = wordSearch(cmdTop(cmdbuf)->text, vocab);

		if(temp == NULL){
			// Append the new word
			temp = newWord(vocab->grow);
			command *to_free = cmdPop(cmdbuf);
			assert(to_free->text != NULL);
			strcpy(temp->name, to_free->text);
			free(to_free->text);
		}else{
			// Wipe old definition
			temp->length = -1;
			cmdDrop(cmdbuf);
		}

	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
		cmdClear(cmdbuf);
		return;
	}

	while(strcmp(";", cmdTop(cmdbuf)->text)){
		// TODO: This could be changed to improve threading
		growWord(temp, cmdPop(cmdbuf)->text, vocab); // Cannot free the text array because it will be used by the word definition hereafter

		if(cmdbuf->top < 0){
			fprintf(stderr,"ERROR: Incomplete definition\n");
			cmdClear(cmdbuf);
			return;
		}
	}
	// Get rid of ";"
	cmdDrop(cmdbuf);

	// Successful definition, print name
	printf("%s\n",temp->name);
*/
	return;
}

// Grows the array defining a word
// TODO rewrite
void growWord(word * word, char * com, dict * vocab){
/*
	coreword * tempcore;
	word->length++;
	if(word->length >= word->capacity) {
		word->capacity = word->length * 2;
		word->array = realloc(word->array, word->capacity * sizeof(command));
	}
	assert(word->length < word->capacity);
	word->array[word->length].text = com;
	// Speedup core words
	tempcore = coreSearch(word->array[word->length].text, vocab);
	if(tempcore != NULL){
		word->array[word->length].func = tempcore->func;
	}else{
		word->array[word->length].func = NULL;
	}
*/
}

void defCore(char * name, void (*func)(stack *, cmdbuffer *, dict*), dict * vocab){
	coreword * temp = NULL;
	if(vocab->core == NULL){
		temp = malloc(sizeof(coreword));
		strcpy(temp->name, name); // FIXME core word names have fixed bound on length
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
