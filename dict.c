#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "dict.h"
#include "elem.h"
#include "util.h"
#include "corewords.h"

// Searches dictionaries, runs a word if possible
// Should add an error when word is not found
elem * wordFind(elem * sequence, elem * stack, dict * vocab){
	int done = 0;
	char * elemName = sequence->chars;
	coreword * tempCore;
	word * tempWord;

	if(elemName[0] == '\0') return stack;

	// Search core dict first
	tempCore = vocab->core;
	do{
		if(!strcmp(tempCore->name, elemName)){
			stack = tempCore->func(stack, sequence); // Run built-in function. sequence provided so that conditionals can conditionally delete adjacent words.
			done = 1;
		}
		tempCore = tempCore->next;
	}while(tempCore != NULL);

	// Search subdicts (for now just one)
	tempWord = vocab->sub->wordlist;

	if(!done && (vocab->sub->wordlist != NULL)){
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
// TODO Should replace if function exists, or error if core function
elem * funcDec(elem * seq, dict * vocab){
	assert(vocab != NULL);
	assert(vocab->sub != NULL);

	// Only allows one word for now
	if (vocab->sub->wordlist == NULL){
		vocab->sub->wordlist = malloc(sizeof(word));
	}

	// Skip over ':'
	if(seq->next != NULL){
		seq = seq->next;
	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
	}
	// Skip over function name
	if(seq->next != NULL){
		strcpy(vocab->sub->wordlist->name, seq->chars); // Name the function
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
			return NULL; // TODO We should free the sequence before returning
		}else{
			seq = seq->next;
		}
	}

	vocab->sub->wordlist->next = NULL;
	// TODO We should free the entire sequence
	printf("%s\n",vocab->sub->wordlist->name);
	return NULL;
}
