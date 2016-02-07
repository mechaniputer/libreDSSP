#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "dict.h"
#include "elem.h"
#include "util.h"
#include "corewords.h"

// Searches dictionaries, runs a word if possible
// Should add an error when word is not found
elem * wordFind(char * elemName, elem * stack, dict * vocab){
	int done = 0;
	coreword * tempCore;
	word * tempWord;

	if(elemName[0] == '\0') return stack;

	// Search core dict first
	tempCore = vocab->core;
	do{
		if(!strcmp(tempCore->name, elemName)){
			stack = tempCore->func(stack); // Run built-in function
			done = 1;
		}
		tempCore = tempCore->next;
	}while(tempCore != NULL);

	// Search subdicts (for now just one)
	tempWord = vocab->sub->wordlist;

	if (vocab->sub->wordlist != NULL){
		do{
			if(!strcmp(tempWord->name, elemName)){
				run(stack, parseInput(tempWord->definition), vocab);
				done = 1;
			}
			tempWord = tempWord->next;
		}while(tempWord != NULL);
	}

	if(!done) fprintf(stderr,"ERROR: %s unrecognized\n",elemName);
	return stack;
}

// Attempts to define a new function
elem * funcDec(elem * seq, dict * vocab){
	assert(vocab != NULL);
	assert(vocab->sub != NULL);

	// Only allows one word for now
	if (vocab->sub->wordlist == NULL){
		vocab->sub->wordlist = malloc(sizeof(word));
	}
	strcpy(vocab->sub->wordlist->name, "foo"); // Name the function

	// Skip over ':'
	if(seq->next != NULL){
		seq = seq->next;
	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
	}
	// Skip over function name
	if(seq->next != NULL){
		seq = seq->next;
	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
	}

	// TODO This limits the size of a declaration and should be dynamic
	vocab->sub->wordlist->definition = malloc(80*sizeof(char));
	while(seq->chars[0] != ';'){
		strcat(vocab->sub->wordlist->definition, seq->chars);
		strcat(vocab->sub->wordlist->definition, " ");
		if(seq->next == NULL){
			fprintf(stderr,"ERROR: Incomplete definition\n");
			return NULL; // We should free the sequence before returning
		}else{
			seq = seq->next;
		}
	}

	vocab->sub->wordlist->next = NULL;
	// TODO We should free the entire sequence
	printf("%s\n",vocab->sub->wordlist->name);
	return NULL;
}
