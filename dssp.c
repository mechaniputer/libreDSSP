#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "dict.h"
#include "elem.h"
#include "corewords.h"

// This is backwards for now
void showStack(elem * stack){
	if(stack == NULL) return;
	elem * temp = stack;
	do{
		printf("%d ",temp->value);
		temp = temp->next;
	}while(temp != NULL);
	printf("\n");
}

// This can be reduced but it hits every case
int isnum(char * foo){
	int i = 0;
	char temp = foo[0];

	if(foo == NULL) return 0;
	if(strlen(foo) == 0) return 0;

	// If first is minus, scan from beginning to see if all are digits
	if(temp=='-'){
		for (i=1; i < (strlen(foo)); i++){
			if(!isdigit(foo[i])) return 0;
		}
	}else for (i=0; i < (strlen(foo)); i++){
		if(!isdigit(foo[i])) return 0;
	}
	return 1;
}

// Eventually this will be used in the main loop
elem * parseInput(char * line){
	char ch;
	int i = 0;
	int j = 0;
	elem * seqHead;
	elem * seqcurr;
	elem * seqtail;

	seqHead = malloc(sizeof(elem));
	seqtail = seqHead;
	seqtail->next = NULL;

	// TODO Not safe!
	while((line[j] != '\0') && (j < 79)){
		ch = line[j++];
		if (ch == '[') {
			seqtail->chars[i++] = '[';
			while(((ch = line[j++]) != ']')){
				if(i>8) break; // TODO This limits a word or comment to 8 chars due to fixed size in struct
				seqtail->chars[i++] = ch;
			}
			seqtail->chars[i++] = ']';

		} else if(ch != ' ') {
			if(i>8) break; // TODO This limits a word or comment to 8 chars due to fixed size in struct
			seqtail->chars[i++] = ch;

		} else if ((ch == ' ') && (i != 0)) { // Handles adjacent spaces
			seqtail->chars[i] = '\0';
			seqcurr = seqtail;
			seqtail = malloc(sizeof(elem));
			seqcurr->next = seqtail; // old tail used to point to NULL, now to new elem
			seqtail->next = NULL;
			i=0;
		}
	}
	seqtail->chars[i] =  '\0';
	return seqHead;
}

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
	elem * tempStack;
	elem * tempSeq;
	elem * seqPrev; // We use this to free each element after we are done reading it

	dict * vocab = malloc(sizeof(dict)); // Contains all recognized words
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add dicts later
	vocab->core = malloc(sizeof(coreword)); // For built-in words

	// Built-ins
	strcpy(vocab->core->name, "+"); // First dict entry
	vocab->core->func = plus;
	vocab->core->next = malloc(sizeof(coreword));
	strcpy(vocab->core->next->name, "bye"); // Second dict entry
	vocab->core->next->func = bye;
	vocab->core->next->next = NULL;

	// Sub-Dictionaries
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;

	stack = NULL;

	while(1){
		// Show prompt, get line of input
		seqHead = parseInput(prompt());
		tempSeq = seqHead;
	
		do{
			assert(tempSeq != NULL);
			if(isnum(tempSeq->chars)){ // Numerical constant
				tempStack = stack;
				stack = malloc(sizeof(elem));
				stack->next = tempStack;
				stack->value = atoi(tempSeq->chars);
			}else if (tempSeq->chars[0] == ':'){ // Function declaration
				tempSeq = funcDec(seqHead,vocab);
			}else if (tempSeq->chars[0] == '['){ // Comment
				// Do nothing
			}else{ // Not a number or a function declaration

				//  The args will need to change to allow branching
				//  because we need to provide choices to the
				//  branching word.
				stack = wordFind(tempSeq->chars, stack, vocab);
			}
			if(tempSeq != NULL){ // This will be NULL if we did a function declaration
				seqPrev = tempSeq;
				tempSeq = tempSeq->next;
				free(seqPrev); // We should be done with this element
			}
		}while(tempSeq != NULL);

		showStack(stack);
	}

	return 0;
}
