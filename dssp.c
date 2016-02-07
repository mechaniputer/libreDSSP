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
	assert(stack != NULL);
	elem * temp = stack;
	do{
		printf("%d ",temp->value);
		temp = temp->next;
	}while(temp != NULL);
	printf("\n");
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
	strcpy(vocab->core->name, "+"); // First dict entry
	vocab->core->func = plus; // fn ptr
	vocab->core->next = NULL;
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;

	stack = NULL;

	seqhead = getSequence();
	tempSeq = seqhead;
	
	do{
		assert(tempSeq != NULL);
		if(isdigit(tempSeq->chars[0])){
			tempStack = stack;
			stack = malloc(sizeof(elem));
			stack->next = tempStack;

			stack->value = atoi(tempSeq->chars);
		}else{
			stack = wordFind(tempSeq->chars, stack, vocab);
			assert(stack != NULL);
		}
		assert(tempSeq != NULL);
		tempSeq = tempSeq->next;
	}while(tempSeq != NULL);

	showStack(stack);

	return 0;
}
