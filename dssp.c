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
	int i=0;
	char temp = foo[0];

	if(foo == NULL) return 0;
	if(strlen(foo) == 0) return 0;

	// If first is minus, scan from beginning to see if all are digits
	if(temp=='-'){
		for (i=1; i < (strlen(foo)); i++){
			printf("check %d\n",i);
			if(!isdigit(foo[i])) return 0;
		}
	}else for (i=0; i < (strlen(foo)); i++){
		printf("check %d\n",i);
		if(!isdigit(foo[i])) return 0;
	}
	return 1;
}

elem * getSequence(){
	char ch;
	int i=0;
	elem * seqhead;
	elem * seqcurr;
	elem * seqtail;

	seqhead = malloc(sizeof(elem));
	seqtail = seqhead;
	seqtail->next = NULL;

	printf("* ");

	while(((ch = getchar()) != '\n')){
		if (ch == '['){
			while(((ch = getchar()) != ']'));
		} else if(ch != ' '){
			if(i>8) break;
			seqtail->chars[i++] = ch;
		} else if (i != 0){ // Handles adjacent spaces
			seqtail->chars[i] =  '\0';
			seqcurr = seqtail;
			seqtail = malloc(sizeof(elem));
			seqcurr->next = seqtail; // old tail used to point to NULL, now to new elem
			seqtail->next = NULL;
			i=0;
		}
	}
	seqtail->chars[i] =  '\0';

	return seqhead;
}

int main(){
	elem * seqhead;
	elem * stack;
	elem * tempStack;
	elem * tempSeq;

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
		seqhead = getSequence();
		tempSeq = seqhead;
	
		do{
			assert(tempSeq != NULL);
			if(isnum(tempSeq->chars)){
				tempStack = stack;
				stack = malloc(sizeof(elem));
				stack->next = tempStack;
				stack->value = atoi(tempSeq->chars);
			}else{
				stack = wordFind(tempSeq->chars, stack, vocab);
			}
			assert(tempSeq != NULL);
			tempSeq = tempSeq->next;
		}while(tempSeq != NULL);

		showStack(stack);
	}

	return 0;
}
