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

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <readline/readline.h>

#include "corewords.h"
#include "dict.h"
#include "stack.h"
#include "cmdbuf.h"

// In utils.h
void run(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

void plus(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	int temp;
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for +\n");
		cmdClear(cmdbuf);
		return;
	}
	temp = pop(stack);
	stack->array[stack->top] += temp;
	return;
}

void multiply(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	int temp1;
	int temp2;
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for *\n");
		cmdClear(cmdbuf);
		return;
	}
	temp1 = pop(stack);
	temp2 = top(stack);
	stack->array[stack->top] = temp1 * temp2;
	return;
}

void minus(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	int temp;
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for -\n");
		cmdClear(cmdbuf);
		return;
	}
	temp = pop(stack);
	stack->array[stack->top] -= temp;
	return;
}

void divide(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	int temp1;
	int temp2;
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for /\n");
		cmdClear(cmdbuf);
		return;
	}
	temp1 = pop(stack);
	temp2 = top(stack);
	stack->array[stack->top] = temp2 / temp1;
	return;
}

void negate(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for NEG\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] = -(stack->array[stack->top]);
	return;
}

void absval(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for ABS\n");
		cmdClear(cmdbuf);
		return;
	} else if(top(stack) < 0) {
		stack->array[stack->top] = -(stack->array[stack->top]);
	}
	return;
}

void plus1(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 1+\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] += 1;
	return;
}

void plus2(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 2+\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] += 2;
	return;
}

void plus3(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 3+\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] += 3;
	return;
}

void plus4(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 4+\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] += 4;
	return;
}

void minus1(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 1-\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] -= 1;
	return;
}

void minus2(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 2-\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] -= 2;
	return;
}

void minus3(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 3-\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] -= 3;
	return;
}

void minus4(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 4-\n");
		cmdClear(cmdbuf);
		return;
	}
	stack->array[stack->top] -= 4;
	return;
}

void bye(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	printf("Exiting libreDSSP\n");
	exit(0);
}

// TODO This will need to be modified to support multiple output modes
// Current mode will be readable from a var in vocab
void showTop(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top > -1) printf("%d\n",top(stack));
	return;
}

void showStack(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	int i;
	if(stack->top < 0){
		printf("[]\n");
		return;
	}

	printf("[");
	for(i=0; i<=(stack->top); i++){
		printf(" %d",stack->array[i]);
	}
	printf("]\n");
	return;
}

// FIXME Placeholder because B10 is currently the only mode we support!
void base10(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	return;
}

void ifplus(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*	if((cmdbuf->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		cmdClear(cmdbuf);
		return;
	}

	if(pop(stack) <= 0){
		cmdDrop(cmdbuf);
	}
*/
	return;
}

void ifzero(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*	if((cmdbuf->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF0\n");
		cmdClear(cmdbuf);
		return;
	}

	if(pop(stack) != 0){
		cmdDrop(cmdbuf);
	}
*/
	return;
}

void ifminus(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*	if((cmdbuf->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		cmdClear(cmdbuf);
		return;
	}

	if(pop(stack) >= 0){
		cmdDrop(cmdbuf);
	}
*/
	return;
}

void branchminus(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	if((cmdbuf->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR-\n");
		cmdClear(cmdbuf);
		return;
	}
	if(pop(stack) < 0){ // Do the first thing
		command * temp = cmdPop(cmdbuf);
		cmdDrop(cmdbuf);
		cmdPush(cmdbuf, temp);
	}else{ // Do the second thing
		cmdDrop(cmdbuf);
	}
*/
	return;
}

void branchzero(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	if((cmdbuf->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR0\n");
		cmdClear(cmdbuf);
		return;
	}
	if(pop(stack) == 0){ // Do the first thing
		command * temp = cmdPop(cmdbuf);
		cmdDrop(cmdbuf);
		cmdPush(cmdbuf, temp);
	}else{ // Do the second thing
		cmdDrop(cmdbuf);
	}
*/
	return;
}

void branchplus(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	if((cmdbuf->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR+\n");
		cmdClear(cmdbuf);
		return;
	}
	if(pop(stack) > 0){ // Do the first thing
		command * temp = cmdPop(cmdbuf);
		cmdDrop(cmdbuf);
		cmdPush(cmdbuf, temp);
	}else{ // Do the second thing
		cmdDrop(cmdbuf);
	}
*/
	return;
}

void branchsign(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	if((cmdbuf->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR0\n");
		cmdClear(cmdbuf);
		return;
	}
	if(top(stack) < 0){ // Do the first thing
		command * temp = cmdPop(cmdbuf);
		cmdDrop(cmdbuf);
		cmdDrop(cmdbuf);
		cmdPush(cmdbuf, temp);
	}else if(top(stack) == 0){ // Do the second thing
		cmdDrop(cmdbuf);
		command * temp = cmdPop(cmdbuf);
		cmdDrop(cmdbuf);
		cmdPush(cmdbuf, temp);
	}else{ // Do the third thing
		cmdDrop(cmdbuf);
		cmdDrop(cmdbuf);
	}
	pop(stack);
*/
	return;
}

void branch(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	command *tempcmd;
	command *branchCom = malloc(sizeof(command));
	branchCom->text = malloc(3*sizeof(char));
	strcpy(branchCom->text, "BR");
	branchCom->func = NULL; // TODO This can be made faster with threading

	if((cmdbuf->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(stack);

	if(!strcmp("ELSE", cmdTop(cmdbuf)->text)){
		pop(stack);
		cmdDrop(cmdbuf);
	}else if(temp == atoi(cmdTop(cmdbuf)->text)){
		if((cmdbuf->top < 3) || (stack->top < 0)){
			fprintf(stderr,"ERROR: Insufficient operands for BR\n");
			cmdClear(cmdbuf);
			return;
		}
		pop(stack);
		cmdDrop(cmdbuf);
		tempcmd = cmdPop(cmdbuf);
		while(strcmp("ELSE", cmdPop(cmdbuf)->text));
		cmdPop(cmdbuf); // FIXME Why is it not safe to free this?
		cmdPush(cmdbuf, tempcmd);
	}else{
		if((cmdbuf->top < 3) || (stack->top < 0)){
			free(branchCom);
			fprintf(stderr,"ERROR: Insufficient operands for BR\n");
			cmdClear(cmdbuf);
			return;
		}
		cmdDrop(cmdbuf);
		cmdDrop(cmdbuf);
		cmdPush(cmdbuf, branchCom);
	}
*/
	return;
}

void equality(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for =\n");
		cmdClear(cmdbuf);
		return;
	}
	int a = pop(stack);
	int b = pop(stack);
	if(b == a){
		push(stack,1);
	}else{
		push(stack,0);
	}
	return;
}

void greaterthan(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for >\n");
		cmdClear(cmdbuf);
		return;
	}
	int a = pop(stack);
	int b = pop(stack);
	if(b > a){
		push(stack,1);
	}else{
		push(stack,0);
	}
	return;
}

void lessthan(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for <\n");
		cmdClear(cmdbuf);
		return;
	}
	int a = pop(stack);
	int b = pop(stack);
	if(b < a){
		push(stack,1);
	}else{
		push(stack,0);
	}
	return;
}

// FIXME frees are tricky here.
// FIXME If the stack grows then any prior references to commands on the stack become invalid.
// FIXME This problem could result in bugs literally anywhere in the interpreter!!!
// FIXME However those problems seem to manifest here especially because it is one of the only core words where the stack is likely to grow a lot.
// TODO There are definitely bugs present in this function which sometimes cause the interpreter to crash.
void doloop(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	int i;
	if((cmdbuf->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for DO\n");
		cmdClear(cmdbuf);
		return;
	}
	command *repeat; // This one will be the master copy
	command *to_free = cmdPop(cmdbuf); // We must free this before the stack gets a chance to grow
	newCommand(to_free, &repeat); // This produces a local copy that can't get realloced away if the cmdbuffer decides to grow
	cmdFree(to_free);

	int reps = pop(stack);

	for(i = 0; i < reps; i++){
		cmdPush(cmdbuf, repeat);
	}
*/
	return;
}

// Stack manipulation
void exch2(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for E2\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - 1];
	stack->array[stack->top - 1] = temp;
	return;
}
void exch3(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 1){
		fprintf(stderr,"ERROR: Insufficient operands for E3\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - 2];
	stack->array[stack->top - 2] = temp;
	return;
}
void exch4(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 2){
		fprintf(stderr,"ERROR: Insufficient operands for E4\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - 3];
	stack->array[stack->top - 3] = temp;
	return;
}
void exchdepth(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 0){ // There need to be two or more operands
		fprintf(stderr,"ERROR: Insufficient operands for ET\n");
		cmdClear(cmdbuf);
		return;
	}
	int depth = pop(stack);
	if (depth == 1) return;
	if(stack->top <= depth-2){
		fprintf(stderr,"ERROR: Insufficient operands for ET\n");
		cmdClear(cmdbuf);
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERROR: Can only exchange from positive depth\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - (depth - 1)];
	stack->array[stack->top - (depth - 1)] = temp;
	return;
}

void copy(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for C\n");
		cmdClear(cmdbuf);
		return;
	}
	push(stack,top(stack));
	return;
}

void copy2(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for C2\n");
		cmdClear(cmdbuf);
		return;
	}
	push(stack, stack->array[stack->top - 1]);
	return;
}
void copy3(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 1){
		fprintf(stderr,"ERROR: Insufficient operands for C3\n");
		cmdClear(cmdbuf);
		return;
	}
	push(stack, stack->array[stack->top - 2]);
	return;
}
void copy4(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 2){
		fprintf(stderr,"ERROR: Insufficient operands for C4\n");
		cmdClear(cmdbuf);
		return;
	}
	push(stack, stack->array[stack->top - 3]);
	return;
}
void copydepth(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top <= 0){ // There need to be two or more operands
		fprintf(stderr,"ERROR: Insufficient operands for CT\n");
		cmdClear(cmdbuf);
		return;
	}
	int depth = pop(stack);
	if(stack->top <= depth-2){
		fprintf(stderr,"ERROR: Insufficient operands for CT\n");
		cmdClear(cmdbuf);
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERROR: Can only copy from positive depth\n");
		cmdClear(cmdbuf);
		return;
	}
	push(stack, stack->array[stack->top - (depth - 1)]);
	return;
}

void drop(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for D\n");
		cmdClear(cmdbuf);
		return;
	}
	pop(stack);
	return;
}

void dropStack(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	stack->top = -1;
	return;
}

// Attempts to define a new variable
void defVar(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for !\n");
		cmdClear(cmdbuf);
		return;
	}

	if(cmdbuf->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for !\n");
		cmdClear(cmdbuf);
		return;
	}

	command * name = cmdPop(cmdbuf);
	int value = pop(stack);
	variable * temp;

	assert(vocab != NULL);

	// See if it is a core word
	if(coreSearch(name->text, vocab)){
		fprintf(stderr,"ERROR: %s is in core dictionary\n",name->text);
		cmdClear(cmdbuf);
		return;
	}

	if(wordSearch(name->text, vocab) != NULL){
		fprintf(stderr,"ERROR: %s is in dictionary\n",name->text);
		cmdClear(cmdbuf);
		return;
	}

	if(vocab->var == NULL){
		vocab->var = malloc(sizeof(variable));
		temp = vocab->var;
		strcpy(temp->name, name->text);
		temp->next = NULL;
	}else{
		temp = varSearch(name->text, vocab);
		if(temp == NULL){
			temp = vocab->var;
			while(temp->next != NULL) temp = temp->next;
			temp->next = malloc(sizeof(variable));
			temp = temp->next;
			strcpy(temp->name, name->text);
			temp->next = NULL;
		}
	}

	temp->value = value;
*/
	return;
}

void printNewline(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	printf("\n");
	return;
}
void printSpace(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	printf(" ");
	return;
}

void listDicts(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	assert(vocab != NULL);
	subdict * tempSub = vocab->sub;
	printf("$PRIME OPEN\n");
	while(tempSub != NULL){
		printf("%s",tempSub->name);
		if(tempSub->open) printf(" OPEN\n");
		else printf(" CLOSED\n");
		tempSub = tempSub->next;
	}
	return;
}

void growSub(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	subdict * tempSub = vocab->sub;

	if(cmdbuf->top < 0){
		fprintf(stderr,"ERROR: Must specify a subvocabulary name\n");
		cmdClear(cmdbuf);
		return;
	}

	if(strncmp(cmdTop(cmdbuf)->text,"$",1)){
		fprintf(stderr,"ERROR: subdictionary must begin with $ character\n");
		cmdClear(cmdbuf);
		return;
	}

	if(!strcmp(cmdTop(cmdbuf)->text,"$PRIME")){
		fprintf(stderr,"ERROR: cannot alter $PRIME subvocabulary\n");
		cmdClear(cmdbuf);
		return;
	}

	while(tempSub != NULL){
		if(!strcmp(cmdTop(cmdbuf)->text, tempSub->name)) {
			break;
		}
		tempSub = tempSub->next;
	}

	if(tempSub == NULL){ // We are making a new subdict
		tempSub = newDict(vocab, cmdTop(cmdbuf)->text);
	}

	vocab->grow = tempSub;
	tempSub->open = 1;
	cmdDrop(cmdbuf);
*/
	return;
}

void shutSub(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	subdict * tempSub = vocab->sub;

	if(cmdbuf->top < 0){
		fprintf(stderr,"ERROR: Must specify a subvocabulary\n");
		cmdClear(cmdbuf);
		return;
	}

	if(!strcmp(cmdTop(cmdbuf)->text,"$PRIME")){
		fprintf(stderr,"ERROR: cannot shut $PRIME subvocabulary\n");
		cmdClear(cmdbuf);
		return;
	}

	while(tempSub != NULL){
		if(!strcmp(tempSub->name, cmdTop(cmdbuf)->text)) break;
		tempSub = tempSub->next;
	}

	if (tempSub == NULL){
		fprintf(stderr,"ERROR: subdictionary %s does not exist\n",cmdTop(cmdbuf)->text);
		cmdClear(cmdbuf);
		return;
	}
	tempSub->open = 0;
	vocab->grow = NULL;
	cmdDrop(cmdbuf);
	return;
*/
}

void openSub(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
/*
	subdict * tempSub = vocab->sub;

	if(cmdbuf->top < 0){
		fprintf(stderr,"ERROR: Must specify a subvocabulary\n");
		cmdClear(cmdbuf);
		return;
	}

	while(tempSub != NULL){
		if(!strcmp(tempSub->name, cmdTop(cmdbuf)->text)) break;
		tempSub = tempSub->next;
	}

	if (tempSub == NULL){
		fprintf(stderr,"ERROR: subdictionary %s does not exist\n",cmdTop(cmdbuf)->text);
		cmdClear(cmdbuf);
		return;
	}
	tempSub->open = 1;
	cmdDrop(cmdbuf);
*/
	return;
}

// TODO Confirm that this is the correct behavior
// TODO Support multiple number bases
void termInNum(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top > -1){ // Requires one operand
		int len = pop(stack);
		if(len < 0){
			fprintf(stderr,"ERROR: TIN requires non-negative operant\n");
			cmdClear(cmdbuf);
			return;
		}
		char * line = readline("");
		if(line){
			int maxlen = strlen(line);
			if(len < maxlen){
				push(stack, atoi(line + ((maxlen-len)*sizeof(char))));
			}else{
				push(stack, atoi(line));
			}
		}else{
			fprintf(stderr,"ERROR: TIN could not read line\n");
			cmdClear(cmdbuf);
			return;
		}
	}else{
		fprintf(stderr,"ERROR: Insufficient operands for TIN\n");
		cmdClear(cmdbuf);
		return;
	}
	return;
}

// TODO Confirm that this is the correct behavior
// TODO Support multiple number bases
void termOutNum(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	if(stack->top > 0){ // Requires two operands
		int len = pop(stack);
		int num = pop(stack);
		int maxlen = snprintf( NULL, 0, "%d", num );
		char *toPrint = malloc( maxlen + 1 );
		snprintf( toPrint, maxlen + 1, "%d", num );


		if(maxlen > len){
			for(int i=0; i < len; i++){
				printf("%c",toPrint[(maxlen-len)+i]);
			}
		}else{
			for(int i=0; i < maxlen; i++){
				printf("%c",toPrint[i]);
			}
		}

		free(toPrint);
	}else{
		fprintf(stderr,"ERROR: Insufficient operands for TON\n");
		cmdClear(cmdbuf);
		return;
	}
	return;
}

void stackDepth(stack * stack, cmdbuffer * cmdbuf, dict * vocab){
	push(stack, (stack->top)+1);
	return;
}


