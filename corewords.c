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
#include "elem.h"
#include "stack.h"

// In utils.h
void run(stack * stack, cmdstack * cmdstack, dict * vocab);

void plus(stack * stack, cmdstack * cmdstack, dict * vocab){
	int temp;
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for +\n");
		cmdClear(cmdstack);
		return;
	}
	temp = pop(stack);
	stack->array[stack->top] += temp;
	return;
}

void multiply(stack * stack, cmdstack * cmdstack, dict * vocab){
	int temp1;
	int temp2;
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for *\n");
		cmdClear(cmdstack);
		return;
	}
	temp1 = pop(stack);
	temp2 = top(stack);
	stack->array[stack->top] = temp1 * temp2;
	return;
}

void minus(stack * stack, cmdstack * cmdstack, dict * vocab){
	int temp;
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for -\n");
		cmdClear(cmdstack);
		return;
	}
	temp = pop(stack);
	stack->array[stack->top] -= temp;
	return;
}

void divide(stack * stack, cmdstack * cmdstack, dict * vocab){
	int temp1;
	int temp2;
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for /\n");
		cmdClear(cmdstack);
		return;
	}
	temp1 = pop(stack);
	temp2 = top(stack);
	stack->array[stack->top] = temp2 / temp1;
	return;
}

void negate(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for NEG\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] = -(stack->array[stack->top]);
	return;
}

void absval(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for ABS\n");
		cmdClear(cmdstack);
		return;
	} else if(top(stack) < 0) {
		stack->array[stack->top] = -(stack->array[stack->top]);
	}
	return;
}

void add1(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 1+\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] += 1;
	return;
}

void add2(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 2+\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] += 2;
	return;
}

void add3(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 3+\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] += 3;
	return;
}

void add4(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 4+\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] += 4;
	return;
}

void sub1(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 1-\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] -= 1;
	return;
}

void sub2(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 2-\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] -= 2;
	return;
}

void sub3(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 3-\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] -= 3;
	return;
}

void sub4(stack * stack, cmdstack * cmdstack, dict * vocab){
	// -1 indicates empty stack
	if(stack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 4-\n");
		cmdClear(cmdstack);
		return;
	}
	stack->array[stack->top] -= 4;
	return;
}

void bye(stack * stack, cmdstack * cmdstack, dict * vocab){
	printf("Exiting libreDSSP\n");
	exit(0);
}

// TODO This will need to be modified to support multiple output modes
// Current mode will be readable from a var in vocab
void showTop(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top > -1) printf("%d\n",top(stack));
	return;
}

void showStack(stack * stack, cmdstack * cmdstack, dict * vocab){
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

void ifplus(stack * stack, cmdstack * cmdstack, dict * vocab){
	if((cmdstack->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		cmdClear(cmdstack);
		return;
	}

	if(pop(stack) <= 0){
		cmdDrop(cmdstack);
	}
	return;
}

void ifzero(stack * stack, cmdstack * cmdstack, dict * vocab){
	if((cmdstack->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF0\n");
		cmdClear(cmdstack);
		return;
	}

	if(pop(stack) != 0){
		cmdDrop(cmdstack);
	}
	return;
}

void ifminus(stack * stack, cmdstack * cmdstack, dict * vocab){
	if((cmdstack->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		cmdClear(cmdstack);
		return;
	}

	if(pop(stack) >= 0){
		cmdDrop(cmdstack);
	}
	return;
}

void branchminus(stack * stack, cmdstack * cmdstack, dict * vocab){
	if((cmdstack->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR-\n");
		cmdClear(cmdstack);
		return;
	}
	if(pop(stack) < 0){ // Do the first thing
		command * temp = cmdPop(cmdstack);
		cmdDrop(cmdstack);
		cmdPush(cmdstack, temp);
	}else{ // Do the second thing
		cmdDrop(cmdstack);
	}
	return;
}

void branchzero(stack * stack, cmdstack * cmdstack, dict * vocab){
	if((cmdstack->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR0\n");
		cmdClear(cmdstack);
		return;
	}
	if(pop(stack) == 0){ // Do the first thing
		command * temp = cmdPop(cmdstack);
		cmdDrop(cmdstack);
		cmdPush(cmdstack, temp);
	}else{ // Do the second thing
		cmdDrop(cmdstack);
	}
	return;
}

void branchplus(stack * stack, cmdstack * cmdstack, dict * vocab){
	if((cmdstack->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR+\n");
		cmdClear(cmdstack);
		return;
	}
	if(pop(stack) > 0){ // Do the first thing
		command * temp = cmdPop(cmdstack);
		cmdDrop(cmdstack);
		cmdPush(cmdstack, temp);
	}else{ // Do the second thing
		cmdDrop(cmdstack);
	}
	return;
}

void branchsign(stack * stack, cmdstack * cmdstack, dict * vocab){
	if((cmdstack->top < 1) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR0\n");
		cmdClear(cmdstack);
		return;
	}
	if(top(stack) < 0){ // Do the first thing
		command * temp = cmdPop(cmdstack);
		cmdDrop(cmdstack);
		cmdDrop(cmdstack);
		cmdPush(cmdstack, temp);
	}else if(top(stack) == 0){ // Do the second thing
		cmdDrop(cmdstack);
		command * temp = cmdPop(cmdstack);
		cmdDrop(cmdstack);
		cmdPush(cmdstack, temp);
	}else{ // Do the third thing
		cmdDrop(cmdstack);
		cmdDrop(cmdstack);
	}
	pop(stack);
	return;
}

void branch(stack * stack, cmdstack * cmdstack, dict * vocab){
	command *tempcmd;
	command *branchCom = malloc(sizeof(struct command));
	branchCom->text = malloc(3*sizeof(char));
	strcpy(branchCom->text, "BR");
	branchCom->func = NULL; // TODO This can be made faster with threading

	if((cmdstack->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR\n");
		cmdClear(cmdstack);
		return;
	}
	int temp = top(stack);

	if(!strcmp("ELSE", cmdTop(cmdstack)->text)){
		pop(stack);
		cmdDrop(cmdstack);
	}else if(temp == atoi(cmdTop(cmdstack)->text)){
		if((cmdstack->top < 3) || (stack->top < 0)){
			fprintf(stderr,"ERROR: Insufficient operands for BR\n");
			cmdClear(cmdstack);
			return;
		}
		pop(stack);
		cmdDrop(cmdstack);
		tempcmd = cmdPop(cmdstack);
		while(strcmp("ELSE", cmdPop(cmdstack)->text));
		cmdPop(cmdstack); // FIXME Why is it not safe to free this?
		cmdPush(cmdstack, tempcmd);
	}else{
		if((cmdstack->top < 3) || (stack->top < 0)){
			free(branchCom);
			fprintf(stderr,"ERROR: Insufficient operands for BR\n");
			cmdClear(cmdstack);
			return;
		}
		cmdDrop(cmdstack);
		cmdDrop(cmdstack);
		cmdPush(cmdstack, branchCom);
	}
	return;
}

// FIXME frees are tricky here.
// FIXME If the stack grows then any prior references to commands on the stack become invalid.
// FIXME This problem could result in bugs literally anywhere in the interpreter!!!
// FIXME However those problems seem to manifest here especially because it is one of the only core words where the stack is likely to grow a lot.
// TODO There are definitely bugs present in this function which sometimes cause the interpreter to crash.
void doloop(stack * stack, cmdstack * cmdstack, dict * vocab){
	int i;
	if((cmdstack->top < 0) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for DO\n");
		cmdClear(cmdstack);
		return;
	}
	command *repeat; // This one will be the master copy
	command *to_free = cmdPop(cmdstack); // We must free this before the stack gets a chance to grow
	newCommand(to_free, &repeat); // This produces a local copy that can't get realloced away if the cmdstack decides to grow
	cmdFree(to_free);

	int reps = pop(stack);

	for(i = 0; i < reps; i++){
		cmdPush(cmdstack, repeat);
	}
	return;
}

// Stack manipulation
void exch2(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for E2\n");
		cmdClear(cmdstack);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - 1];
	stack->array[stack->top - 1] = temp;
	return;
}
void exch3(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top <= 1){
		fprintf(stderr,"ERROR: Insufficient operands for E3\n");
		cmdClear(cmdstack);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - 2];
	stack->array[stack->top - 2] = temp;
	return;
}
void exch4(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top <= 2){
		fprintf(stderr,"ERROR: Insufficient operands for E4\n");
		cmdClear(cmdstack);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - 3];
	stack->array[stack->top - 3] = temp;
	return;
}
void exchdepth(stack * stack, cmdstack * cmdstack, dict * vocab){
	int depth = pop(stack);
	if (depth == 1) return;
	if(stack->top <= depth-2){
		fprintf(stderr,"ERROR: Insufficient operands for ET\n");
		cmdClear(cmdstack);
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERROR: Can only exchange from positive depth\n");
		cmdClear(cmdstack);
		return;
	}
	int temp = top(stack);
	stack->array[stack->top] = stack->array[stack->top - (depth - 1)];
	stack->array[stack->top - (depth - 1)] = temp;
	return;
}

void copy(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for C\n");
		cmdClear(cmdstack);
		return;
	}
	push(stack,top(stack));
	return;
}

void copy2(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for C2\n");
		cmdClear(cmdstack);
		return;
	}
	push(stack, stack->array[stack->top - 1]);
	return;
}
void copy3(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top <= 1){
		fprintf(stderr,"ERROR: Insufficient operands for C3\n");
		cmdClear(cmdstack);
		return;
	}
	push(stack, stack->array[stack->top - 2]);
	return;
}
void copy4(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top <= 2){
		fprintf(stderr,"ERROR: Insufficient operands for C4\n");
		cmdClear(cmdstack);
		return;
	}
	push(stack, stack->array[stack->top - 3]);
	return;
}
void copydepth(stack * stack, cmdstack * cmdstack, dict * vocab){
	int depth = pop(stack);
	if(stack->top <= depth-2){
		fprintf(stderr,"ERROR: Insufficient operands for CT\n");
		cmdClear(cmdstack);
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERROR: Can only copy from positive depth\n");
		cmdClear(cmdstack);
		return;
	}
	push(stack, stack->array[stack->top - (depth - 1)]);
	return;
}

void drop(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for D\n");
		cmdClear(cmdstack);
		return;
	}
	pop(stack);
	return;
}

void dropStack(stack * stack, cmdstack * cmdstack, dict * vocab){
	stack->top = -1;
	return;
}

// Attempts to define a new variable
void defVar(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for !\n");
		cmdClear(cmdstack);
		return;
	}

	if(cmdstack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for !\n");
		cmdClear(cmdstack);
		return;
	}

	command * name = cmdPop(cmdstack);
	int value = pop(stack);
	variable * temp;

	assert(vocab != NULL);

	// See if it is a core word
	if(coreSearch(name->text, vocab)){
		fprintf(stderr,"ERROR: %s is in core dictionary\n",name->text);
		cmdClear(cmdstack);
		return;
	}

	if(wordSearch(name->text, vocab) != NULL){
		fprintf(stderr,"ERROR: %s is in dictionary\n",name->text);
		cmdClear(cmdstack);
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
	return;
}

void printNewline(stack * stack, cmdstack * cmdstack, dict * vocab){
	printf("\n");
	return;
}
void printSpace(stack * stack, cmdstack * cmdstack, dict * vocab){
	printf(" ");
	return;
}

void listDicts(stack * stack, cmdstack * cmdstack, dict * vocab){
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

void growSub(stack * stack, cmdstack * cmdstack, dict * vocab){
	subdict * tempSub = vocab->sub;

	if(cmdstack->top < 0){
		fprintf(stderr,"ERROR: Must specify a subvocabulary name\n");
		cmdClear(cmdstack);
		return;
	}

	if(strncmp(cmdTop(cmdstack)->text,"$",1)){
		fprintf(stderr,"ERROR: subdictionary must begin with $ character\n");
		cmdClear(cmdstack);
		return;
	}

	if(!strcmp(cmdTop(cmdstack)->text,"$PRIME")){
		fprintf(stderr,"ERROR: cannot alter $PRIME subvocabulary\n");
		cmdClear(cmdstack);
		return;
	}

	while(tempSub != NULL){
		if(!strcmp(cmdTop(cmdstack)->text, tempSub->name)) {
			break;
		}
		tempSub = tempSub->next;
	}

	if(tempSub == NULL){ // We are making a new subdict
		tempSub = newDict(vocab, cmdTop(cmdstack)->text);
	}

	vocab->grow = tempSub;
	tempSub->open = 1;
	cmdDrop(cmdstack);
	return;
}

void shutSub(stack * stack, cmdstack * cmdstack, dict * vocab){
	subdict * tempSub = vocab->sub;

	if(cmdstack->top < 0){
		fprintf(stderr,"ERROR: Must specify a subvocabulary\n");
		cmdClear(cmdstack);
		return;
	}

	if(!strcmp(cmdTop(cmdstack)->text,"$PRIME")){
		fprintf(stderr,"ERROR: cannot shut $PRIME subvocabulary\n");
		cmdClear(cmdstack);
		return;
	}

	while(tempSub != NULL){
		if(!strcmp(tempSub->name, cmdTop(cmdstack)->text)) break;
		tempSub = tempSub->next;
	}

	if (tempSub == NULL){
		fprintf(stderr,"ERROR: subdictionary %s does not exist\n",cmdTop(cmdstack)->text);
		cmdClear(cmdstack);
		return;
	}
	tempSub->open = 0;
	vocab->grow = NULL;
	cmdDrop(cmdstack);
	return;
}

void openSub(stack * stack, cmdstack * cmdstack, dict * vocab){
	subdict * tempSub = vocab->sub;

	if(cmdstack->top < 0){
		fprintf(stderr,"ERROR: Must specify a subvocabulary\n");
		cmdClear(cmdstack);
		return;
	}

	while(tempSub != NULL){
		if(!strcmp(tempSub->name, cmdTop(cmdstack)->text)) break;
		tempSub = tempSub->next;
	}

	if (tempSub == NULL){
		fprintf(stderr,"ERROR: subdictionary %s does not exist\n",cmdTop(cmdstack)->text);
		cmdClear(cmdstack);
		return;
	}
	tempSub->open = 1;
	cmdDrop(cmdstack);
	return;
}

// TODO Confirm that this is the correct behavior
// TODO Support multiple number bases
void termInNum(stack * stack, cmdstack * cmdstack, dict * vocab){
	if(stack->top > -1){ // Requires one operand
		int len = pop(stack);
		if(len < 0){
			fprintf(stderr,"ERROR: TIN requires non-negative operant\n");
			cmdClear(cmdstack);
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
			cmdClear(cmdstack);
			return;
		}
	}else{
		fprintf(stderr,"ERROR: Insufficient operands for TIN\n");
		cmdClear(cmdstack);
		return;
	}
	return;
}

// TODO Confirm that this is the correct behavior
// TODO Support multiple number bases
void termOutNum(stack * stack, cmdstack * cmdstack, dict * vocab){
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
		cmdClear(cmdstack);
		return;
	}
	return;
}

void stackDepth(stack * stack, cmdstack * cmdstack, dict * vocab){
	push(stack, (stack->top)+1);
	return;
}
