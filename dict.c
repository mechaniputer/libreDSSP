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
			// Run built-in function. sequence provided so that conditionals can conditionally delete adjacent words.
			stack = tempCore->func(stack, sequence);
			done = 1;
		}
		tempCore = tempCore->next;
	}while(tempCore != NULL);

	if(!done && (vocab->sub->wordlist != NULL)){
		// Search subdicts (for now just one)
		tempWord = vocab->sub->wordlist;
		do{
			if(!strcmp(tempWord->name, elemName)){
				// run programmed word
				stack = run(stack, parseInput(tempWord->definition), vocab);
				done = 1;
			}
			tempWord = tempWord->next;
		}while(tempWord != NULL);
	}

	if(!done) fprintf(stderr,"ERROR: %s unrecognized\n",elemName);
	return stack;
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

// Attempts to define a new function
// TODO Should replace if function exists, or error if core function
elem * funcDec(elem * seq, dict * vocab){
	word * temp;
	assert(vocab != NULL);
	assert(vocab->sub != NULL);

	// Append the new word
	temp = newWord(vocab->sub);

	// Skip over ':'
	if(seq->next != NULL){
		seq = seq->next;
	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
	}
	// Skip over function name
	if(seq->next != NULL){
		strcpy(temp->name, seq->chars);
		seq = seq->next;
	}else{
		fprintf(stderr,"ERROR: Incomplete definition\n");
	}

	// TODO This limits the size of a declaration and should be dynamic
	while(seq->chars[0] != ';'){
		strcat(temp->definition, seq->chars); // SEGFAULT
		strcat(temp->definition, " ");
		if(seq->next == NULL){
			fprintf(stderr,"ERROR: Incomplete definition\n");
			return NULL; // TODO We should free the sequence before returning
		}else{
			seq = seq->next;
		}
	}
	printf("%s\n",temp->name);

	// TODO We should free the entire sequence
	return NULL;
}
