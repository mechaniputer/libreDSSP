#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "dict.h"
#include "elem.h"
#include "corewords.h"
#include "util.h"

char * prompt(){
	int i = 0;
	char * line = malloc(80 * sizeof(char)); // TODO: This limits line size to 80!
	char ch;
	printf("* ");
	while(((ch = getchar()) != '\n') && (i < 79)){
		line[i++] = ch;
	}
	line[i] = '\0';
	return line;
}

int main(){
	elem * seqHead;
	elem * stack;

	dict * vocab = malloc(sizeof(dict)); // Contains all recognized words
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add dicts later

	// Built-ins
	vocab->core = malloc(sizeof(coreword)); // For built-in words

	strcpy(vocab->core->name, "+"); // First dict entry
	vocab->core->func = plus;

	vocab->core->next = malloc(sizeof(coreword));
	strcpy(vocab->core->next->name, "bye"); // Second dict entry
	vocab->core->next->func = bye;

	vocab->core->next->next = malloc(sizeof(coreword)); // Third dict entry
	strcpy(vocab->core->next->next->name, "."); // Second dict entry
	vocab->core->next->next->func = showTop;

	vocab->core->next->next->next = malloc(sizeof(coreword)); // Fourth dict entry
	strcpy(vocab->core->next->next->next->name, ".."); // Second dict entry
	vocab->core->next->next->next->func = showStack;
	vocab->core->next->next->next->next = NULL;

	// Sub-Dictionaries
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;

	stack = NULL;

	while(1){
		// Show prompt, get line of input
		seqHead = parseInput(prompt());
		stack = run(stack, seqHead, vocab);
	
	}
	return 0;
}
