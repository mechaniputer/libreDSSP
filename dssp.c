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

void defCore(char * name, elem * (*func)(elem *, elem*), dict * vocab){
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

int main(){
	elem * seqHead;
	elem * stack = NULL;

	dict * vocab = malloc(sizeof(dict)); // Contains all recognized words
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add dicts later

	// Built-ins
	vocab->core = malloc(sizeof(coreword)); // For built-in words

	defCore("+", plus, vocab);
	defCore("-", minus, vocab);
	defCore("BYE", bye, vocab);
	defCore(".", showTop, vocab);
	defCore("..", showStack, vocab);
	defCore("IF+", ifplus, vocab);
	defCore("IF0", ifzero, vocab);
	defCore("IF-", ifminus, vocab);

	// Sub-Dictionaries
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;

	while(1){
		// Show prompt, get line of input
		seqHead = parseInput(prompt());
		stack = run(stack, seqHead, vocab);
	}
	return 0;
}
