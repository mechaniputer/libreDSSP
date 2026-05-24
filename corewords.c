/*	This file is part of libreDSSP.

	Copyright 2026 Alan Beadle

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
#include "util.h"

extern void debug();

extern codeword_t * current_codeword;
extern stack *returnStack;
extern dict * vocab;

void plus(){
	int temp;
	// -1 indicates empty dataStack
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for +\n");
		cmdClear(cmdbuf);
		return;
	}
	temp = pop(dataStack);
	dataStack->array[dataStack->top] += temp;
	return;
}

void multiply(){
	int temp1;
	int temp2;
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for *\n");
		cmdClear(cmdbuf);
		return;
	}
	temp1 = pop(dataStack);
	temp2 = top(dataStack);
	dataStack->array[dataStack->top] = temp1 * temp2;
	return;
}

void minus(){
	int temp;
	// -1 indicates empty dataStack
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for -\n");
		cmdClear(cmdbuf);
		return;
	}
	temp = pop(dataStack);
	dataStack->array[dataStack->top] -= temp;
	return;
}

void divide(){
	int temp1;
	int temp2;
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for /\n");
		cmdClear(cmdbuf);
		return;
	}
	if(top(dataStack) == 0){
		fprintf(stderr,"ERROR: Division by zero\n");
		cmdClear(cmdbuf);
	}else{
		temp1 = dataStack->array[dataStack->top];
		temp2 = dataStack->array[dataStack->top-1];
		dataStack->array[dataStack->top-1] = temp2 / temp1;
		dataStack->array[dataStack->top] = temp2 % temp1;
	}
	return;
}

void negate(){
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for NEG\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] = -(dataStack->array[dataStack->top]);
	return;
}

void absval(){
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for ABS\n");
		cmdClear(cmdbuf);
		return;
	} else if(top(dataStack) < 0) {
		dataStack->array[dataStack->top] = -(dataStack->array[dataStack->top]);
	}
	return;
}

void plus1(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 1+\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] += 1;
	return;
}

void plus2(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 2+\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] += 2;
	return;
}

void plus3(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 3+\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] += 3;
	return;
}

void plus4(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 4+\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] += 4;
	return;
}

void minus1(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 1-\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] -= 1;
	return;
}

void minus2(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 2-\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] -= 2;
	return;
}

void minus3(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 3-\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] -= 3;
	return;
}

void minus4(){
	// -1 indicates empty dataStack
	if(dataStack->top == -1){
		fprintf(stderr,"ERROR: Insufficient operands for 4-\n");
		cmdClear(cmdbuf);
		return;
	}
	dataStack->array[dataStack->top] -= 4;
	return;
}

void bye(){
	printf("Exiting libreDSSP\n");
	exit(0);
}

// TODO This will need to be modified to support multiple output modes
// Current mode will be readable from a flag in cmdbuf?
void showTop(){
	if(dataStack->top > -1){
		printf("%ld\n",top(dataStack));
	}else{
		printf("(NIL)\n");
	}
	return;
}

void showStack(){
	printf("[");
	for(int i=0; i<=(dataStack->top); i++){
		if(i != 0) printf(" ");
		printf("%ld",dataStack->array[i]);
	}
	printf("]\n");
	return;
}

// FIXME Placeholder because B10 is currently the only mode we support!
void base10(){
	return;
}

void ifplus(){
	if(((cmdbuf->size - cmdbuf->ip) < 2) || (dataStack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) <= 0){
		cmdbuf->ip += 1; // Skip the first thing
	}

	return;
}

void ifzero(){
	if(((cmdbuf->size - cmdbuf->ip) < 2) || (dataStack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF0\n");
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) != 0){
		cmdbuf->ip += 1; // Skip the first thing
	}

	return;
}

void ifminus(){
	if(((cmdbuf->size - cmdbuf->ip) < 2) || (dataStack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF-\n");
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) >= 0){
		cmdbuf->ip += 1; // Skip the first thing
	}

	return;
}

// This helper replaces common code in BR-, BR0, and BR+ to reduce code duplication and make it easier to maintain.
static inline void branch_helper(codeword_t *outcome){
	if(outcome->user == 1){
		// Push IP to return to after branch completes
		push(returnStack, (intptr_t) (cmdbuf->ip)+2);
		push(returnStack, (intptr_t) cmdbuf->array);
		// Set branch IP
		cmdbuf->array = (codeword_t **) (outcome->data);
		cmdbuf->ip = -1; // The loop in word_next() will increment this to 0
	}else{ // Literal, print, or core
		current_codeword = outcome; // Important for pushLiteral to reference the correct data field
		(*outcome->xt)();
		cmdbuf->ip += 2;
	}
}

// Similar to branch_helper but for the three-way branch in BRS. Main difference is that the IP increment is 3 instead of 2.
static inline void brs_helper(codeword_t *outcome){
	if(outcome->user == 1){
		// Push IP to return to after branch completes
		push(returnStack, (intptr_t) (cmdbuf->ip)+3);
		push(returnStack, (intptr_t) cmdbuf->array);
		// Set branch IP
		cmdbuf->array = (codeword_t **) (outcome->data);
		cmdbuf->ip = -1; // The loop in word_next() will increment this to 0
	}else{ // Literal, print, or core
		current_codeword = outcome; // Important for pushLiteral to reference the correct data field
		(*outcome->xt)();
		cmdbuf->ip += 3;
	}
}

void branchminus(){
	printf("In branchminus()\n");
	if((cmdbuf->size - cmdbuf->ip) < 2){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BR-\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BR-\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) < 0){ // Do the first thing
		printf("Doing first thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else{
		printf("Doing second thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}

	return; // to word_next()
}

void branchzero(){
	printf("In branchzero()\n");
	if((cmdbuf->size - cmdbuf->ip) < 2){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BR0\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BR0\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) == 0){ // Do the first thing
		printf("Doing first thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else{
		printf("Doing second thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}

	return; // to word_next()
}

void branchplus(){
	printf("In branchplus()\n");
	if((cmdbuf->size - cmdbuf->ip) < 2){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BR+\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BR+\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) > 0){ // Do the first thing
		printf("Doing first thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else{
		printf("Doing second thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}

	return; // to word_next()
}

void branchsign(){
	printf("In branchsign()\n");
	if((cmdbuf->size - cmdbuf->ip) < 3){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BRS\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BRS\n");
		debug();
		cmdClear(cmdbuf);
		return;
	}
	showStack();
	intptr_t temp = pop(dataStack);
	if(temp < 0){ // Do the first thing
		printf("Doing first thing\n");
		brs_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else if(temp == 0){ // Do the second thing
		printf("Doing second thing\n");
		brs_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}else{ // Do the third thing
		printf("Doing third thing\n");
		brs_helper(cmdbuf->array[(cmdbuf->ip)+3]);
	}
	return; // to word_next()
}

void branch(){
/*
	command *tempcmd;
	command *branchCom = malloc(sizeof(command));
	branchCom->text = malloc(3*sizeof(char));
	strcpy(branchCom->text, "BR");
	branchCom->func = NULL; // TODO This can be made faster with threading

	if((cmdbuf->top < 0) || (dataStack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for BR\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(dataStack);

	if(!strcmp("ELSE", cmdTop(cmdbuf)->text)){
		pop(dataStack);
		cmdDrop(cmdbuf);
	}else if(temp == atoi(cmdTop(cmdbuf)->text)){
		if((cmdbuf->top < 3) || (dataStack->top < 0)){
			fprintf(stderr,"ERROR: Insufficient operands for BR\n");
			cmdClear(cmdbuf);
			return;
		}
		pop(dataStack);
		cmdDrop(cmdbuf);
		tempcmd = cmdPop(cmdbuf);
		while(strcmp("ELSE", cmdPop(cmdbuf)->text));
		cmdPop(cmdbuf); // FIXME Why is it not safe to free this?
		cmdPush(cmdbuf, tempcmd);
	}else{
		if((cmdbuf->top < 3) || (dataStack->top < 0)){
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

void equality(){
	// -1 indicates empty dataStack
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for =\n");
		cmdClear(cmdbuf);
		return;
	}
	int a = pop(dataStack);
	int b = pop(dataStack);
	if(b == a){
		push(dataStack,1);
	}else{
		push(dataStack,0);
	}
	return;
}

void greaterthan(){
	// -1 indicates empty dataStack
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for >\n");
		cmdClear(cmdbuf);
		return;
	}
	int a = pop(dataStack);
	int b = pop(dataStack);
	if(b > a){
		push(dataStack,1);
	}else{
		push(dataStack,0);
	}
	return;
}

void lessthan(){
	// -1 indicates empty dataStack
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for <\n");
		cmdClear(cmdbuf);
		return;
	}
	int a = pop(dataStack);
	int b = pop(dataStack);
	if(b < a){
		push(dataStack,1);
	}else{
		push(dataStack,0);
	}
	return;
}


// This coreword requires one data stack operand [n] and one word operand (the next word in the command buffer).
// The data operand [n] is popped. The command operand is executed n times.
void doloop(){
	//printf("In doloop()\n");
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for DO\n");
		cmdClear(cmdbuf);
		return;
	}
	if((cmdbuf->size - cmdbuf->ip) < 1){
		fprintf(stderr,"ERROR: Insufficient command operands for DO\n");
		cmdClear(cmdbuf);
		return;
	}

	// For asserts below:
	int snapshot_return_stack_top = returnStack->top; // We will use this to ensure that the return stack is balanced after the loop completes
	codeword_t ** snapshot_cmdbuf_array = cmdbuf->array; // We will use this to ensure that the correct command buffer is active after the loop completes

	int count = pop(dataStack);
	int do_ip = cmdbuf->ip; // Preserve this for loop repetitions
	codeword_t * command = cmdbuf->array[cmdbuf->ip+1];
	if(command->user == 1){
		// For user words.
		for(int i=0; i<count; i++){
			assert(cmdbuf->array == snapshot_cmdbuf_array);
			assert(returnStack->top == snapshot_return_stack_top);

			cmdbuf->ip = do_ip+1; // word_enter() needs the IP to point to the command operand.
			current_codeword = command; // Important for pushLiteral to reference the correct data field

			(*command->xt)(); // word_enter()

			// We cannot use word_next() because it will call word_exit() and proceed to the next word in the command buffer.
			// This code is a near-duplicate of the loop in word_next(), but with some key differences.
			cmdbuf->ip = 0;
			int stop_ip = cmdbuf->size-1; // This stops before word_exit().
			assert(cmdbuf->array[stop_ip]->xt == word_exit);
			while(cmdbuf->ip < stop_ip){
				current_codeword = cmdbuf->array[cmdbuf->ip];
				(*current_codeword->xt)();
				cmdbuf->ip++;
			}
			word_exit(); // Only called when we are done looping the word.
		}
	}else{
		// For non-user words.
		cmdbuf->ip = do_ip+1; // The command itself won't change IP so this can be outside the loop.
		for(int i=0; i<count; i++){
			showStack();
			current_codeword = command; // Important for pushLiteral to reference the correct data field
			(*command->xt)();
			printf("Returned from command execution\n");
		}
	}
	// word_next() will increment the IP again after this,
	cmdbuf->ip = do_ip+1;
	// Assert that the correct cmdbuf is active
	assert(cmdbuf->array == snapshot_cmdbuf_array);
	// Ensure that the return stack is balanced after the loop
	assert(returnStack->top == snapshot_return_stack_top);
	return;
}

// Stack manipulation
void exch2(){
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for E2\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(dataStack);
	dataStack->array[dataStack->top] = dataStack->array[dataStack->top - 1];
	dataStack->array[dataStack->top - 1] = temp;
	return;
}
void exch3(){
	if(dataStack->top <= 1){
		fprintf(stderr,"ERROR: Insufficient operands for E3\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(dataStack);
	dataStack->array[dataStack->top] = dataStack->array[dataStack->top - 2];
	dataStack->array[dataStack->top - 2] = temp;
	return;
}
void exch4(){
	if(dataStack->top <= 2){
		fprintf(stderr,"ERROR: Insufficient operands for E4\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(dataStack);
	dataStack->array[dataStack->top] = dataStack->array[dataStack->top - 3];
	dataStack->array[dataStack->top - 3] = temp;
	return;
}
void exchdepth(){
	if(dataStack->top <= 0){ // There need to be two or more operands
		fprintf(stderr,"ERROR: Insufficient operands for ET\n");
		cmdClear(cmdbuf);
		return;
	}
	int depth = pop(dataStack);
	if (depth == 1) return;
	if(dataStack->top <= depth-2){
		fprintf(stderr,"ERROR: Insufficient operands for ET\n");
		cmdClear(cmdbuf);
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERROR: Can only exchange from positive depth\n");
		cmdClear(cmdbuf);
		return;
	}
	int temp = top(dataStack);
	dataStack->array[dataStack->top] = dataStack->array[dataStack->top - (depth - 1)];
	dataStack->array[dataStack->top - (depth - 1)] = temp;
	return;
}

void copy(){
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for C\n");
		cmdClear(cmdbuf);
		return;
	}
	push(dataStack,top(dataStack));
	return;
}

void copy2(){
	if(dataStack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for C2\n");
		cmdClear(cmdbuf);
		return;
	}
	push(dataStack, dataStack->array[dataStack->top - 1]);
	return;
}
void copy3(){
	if(dataStack->top <= 1){
		fprintf(stderr,"ERROR: Insufficient operands for C3\n");
		cmdClear(cmdbuf);
		return;
	}
	push(dataStack, dataStack->array[dataStack->top - 2]);
	return;
}
void copy4(){
	if(dataStack->top <= 2){
		fprintf(stderr,"ERROR: Insufficient operands for C4\n");
		cmdClear(cmdbuf);
		return;
	}
	push(dataStack, dataStack->array[dataStack->top - 3]);
	return;
}
void copydepth(){
	if(dataStack->top <= 0){ // There need to be two or more operands
		fprintf(stderr,"ERROR: Insufficient operands for CT\n");
		cmdClear(cmdbuf);
		return;
	}
	int depth = pop(dataStack);
	if(dataStack->top <= depth-2){
		fprintf(stderr,"ERROR: Insufficient operands for CT\n");
		cmdClear(cmdbuf);
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERROR: Can only copy from positive depth\n");
		cmdClear(cmdbuf);
		return;
	}
	push(dataStack, dataStack->array[dataStack->top - (depth - 1)]);
	return;
}

void drop(){
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for D\n");
		cmdClear(cmdbuf);
		return;
	}
	pop(dataStack);
	return;
}

void dropStack(){
	dataStack->top = -1;
	return;
}

void pushLiteral(){
	//printf("Pushing %ld\n",current_codeword->data);
	push(dataStack, current_codeword->data);
	return;
}

// Attempts to define a new variable
// The variable name will be taken from the data field of the current codeword, and the variable value will be taken from the top of the data stack.
void defVar(){
/*
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient stack operands for !\n");
		cmdClear(cmdbuf);
		return;
	}

	if(cmdbuf->top < 0){
		fprintf(stderr,"ERROR: Variable name required for !\n");
		cmdClear(cmdbuf);
		return;
	}

	command * name = cmdPop(cmdbuf);
	int value = pop(dataStack);
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

void printNewline(){
	printf("\n");
	return;
}
void printSpace(){
	printf(" ");
	return;
}

void listDicts(){
	assert(vocab != NULL);
	subdict * tempSub = vocab->sub;
	if(vocab->grow == NULL || vocab->grow->open == 0){
		printf("Warning: no dictionary selected for expansion\n");
		printf("%s CLOSED\n",vocab->grow->name);
	}else{
		printf("%s OPEN\n",vocab->grow->name);
	}
	printf("$PRIME OPEN\n");
	while(tempSub != NULL){
		if(tempSub != vocab->grow){
			printf("%s",tempSub->name);
			if(tempSub->open) printf(" OPEN\n");
			else printf(" CLOSED\n");
		}
		tempSub = tempSub->next;
	}
	return;
}

// The GROW command will use the data field of the current codeword to locate the subdict to grow.
// If the subdict doesn't exist, it will be created. If the subdict is closed, it will be reopened.
void growSub(){
	// The dictionary name is in the text field of the current codeword.
	// The subdict must begin with a $ character, and it cannot be $PRIME.
	if(strncmp(current_codeword->text,"$",1)){
		fprintf(stderr,"ERROR: subdictionary must begin with $ character\n");
		cmdClear(cmdbuf);
		return;
	}
	if(!strcmp(current_codeword->text,"$PRIME")){
		fprintf(stderr,"ERROR: cannot alter $PRIME subvocabulary\n");
		cmdClear(cmdbuf);
		return;
	}

	// We need to check if the subdict exists using findDict(), and if it does we need to check if it is open.
	// If it doesn't exist, we need to create it and open it.
	subdict * tempSub = findDict(vocab, current_codeword->text);

	if(tempSub == NULL){ // We are making a new subdict
		tempSub = newDict(vocab, current_codeword->text);
	}

	vocab->grow = tempSub;
	tempSub->open = 1;
	return;
}

// As with GROW, the SHUT command will use the data field of the current codeword to locate the subdict to shut.
// If the subdict doesn't exist, an error is printed. If the subdict is already closed, nothing happens.
// If the subdict is currently selected for growth, it will be deselected.
void shutSub(){
	// The dictionary name is in the text field of the current codeword.
	// The subdict must begin with a $ character, and it cannot be $PRIME.
	if(strncmp(current_codeword->text,"$",1)){
		fprintf(stderr,"ERROR: subdictionary must begin with $ character\n");
		cmdClear(cmdbuf);
		return;
	}
	if(!strcmp(current_codeword->text,"$PRIME")){
		fprintf(stderr,"ERROR: cannot shut $PRIME subvocabulary\n");
		cmdClear(cmdbuf);
		return;
	}

	subdict * tempSub = findDict(vocab, current_codeword->text);
	if (tempSub == NULL){
		fprintf(stderr,"ERROR: subdictionary %s does not exist\n",current_codeword->text);
		cmdClear(cmdbuf);
		return;
	}
	tempSub->open = 0;
	return;
}

// As with GROW, the OPEN command will use the data field of the current codeword to locate the subdict to open.
// If the subdict doesn't exist, an error is printed. If the subdict is already open, nothing happens.
void openSub(){
	// The dictionary name is in the text field of the current codeword.
	// The subdict must begin with a $ character, and it cannot be $PRIME.
	if(strncmp(current_codeword->text,"$",1)){
		fprintf(stderr,"ERROR: subdictionary must begin with $ character\n");
		cmdClear(cmdbuf);
		return;
	}
	if(!strcmp(current_codeword->text,"$PRIME")){
		printf("Warning: $PRIME is always open\n");
		return;
	}

	subdict * tempSub = findDict(vocab, current_codeword->text);
	if (tempSub == NULL){
		fprintf(stderr,"ERROR: subdictionary %s does not exist\n",current_codeword->text);
		cmdClear(cmdbuf);
		return;
	}
	tempSub->open = 1;
	return;
}

// TODO Confirm that this is the correct behavior
// TODO Support multiple number bases
void termInNum(){
	if(dataStack->top > -1){ // Requires one operand
		int len = pop(dataStack);
		if(len < 0){
			fprintf(stderr,"ERROR: TIN requires non-negative operand\n");
			cmdClear(cmdbuf);
			return;
		}
		char * line = readline("");
		if(line){
			int maxlen = strlen(line);
			if(len < maxlen){
				push(dataStack, atoi(line + ((maxlen-len)*sizeof(char))));
			}else{
				push(dataStack, atoi(line));
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
void termOutNum(){
	if(dataStack->top > 0){ // Requires two operands
		int len = pop(dataStack);
		int num = pop(dataStack);
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

void termOutString(){
	// Requires two operands. The top of the stack is a count and below that is a pointer to the string.
	if(dataStack->top < 1){
		fprintf(stderr,"ERROR: Insufficient stack operands for TOS\n");
		cmdClear(cmdbuf);
		return;
	}
	// Pop the count and pointer from the stack
	int len = pop(dataStack);
	char * str = (char *) pop(dataStack);
	// Print exactly the specified number of characters from the string
	for(int i=0; i<len; i++){
		printf("%c", str[i]);
	}
	return;
}

void stackDepth(){
	push(dataStack, (dataStack->top)+1);
	return;
}

// Lists user-defined words in open subdicts.
void inventoryWords(){
	subdict * tempSub = vocab->sub;
	codeword_t * tempWord;
	while(tempSub != NULL){
		if(tempSub->open){
			printf("Subdict: %s\n",tempSub->name);
			tempWord = tempSub->wordlist;
			while(tempWord != NULL){
				printf("  %s",tempWord->name);
				printf("  %s\n",tempWord->text);
				tempWord = tempWord->next;
			}
		}
		tempSub = tempSub->next;
	}
	return;
}