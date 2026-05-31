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
extern stack * returnStack;
extern stack * loopStack;
extern dict * vocab;


void push_one(){
	push(dataStack, 1);
}

void push_two(){
	push(dataStack, 2);
}

void push_four(){
	push(dataStack, 4);
}

void push_eight(){
	push(dataStack, 8);
}

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


// Example code:    BR- FOO BAR
// Compiled result: BR- FOO SKP1 BAR
// If taken, do nothing.
// If not taken, ip+=2
void branchminus(){
	//printf("In branchminus()\n");
	if((cmdbuf->size - cmdbuf->ip) < 2){
			fprintf(stderr,"ERROR: Insufficient branch outcomes for BR-\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BR-\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) >= 0){
		cmdbuf->ip += 2;
	}

	return;
}

// Example code:    BR0 FOO BAR
// Compiled result: BR0 FOO SKP1 BAR
// If taken, do nothing.
// If not taken, ip+=2
void branchzero(){
	//printf("In branchzero()\n");
	if((cmdbuf->size - cmdbuf->ip) < 2){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BR0\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BR0\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) != 0){
		cmdbuf->ip += 2;
	}

	return;
}

// Example code:    BR+ FOO BAR
// Compiled result: BR+ FOO SKP1 BAR
// If taken, do nothing.
// If not taken, ip++
void branchplus(){
	//printf("In branchplus()\n");
	if((cmdbuf->size - cmdbuf->ip) < 2){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BR+\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BR+\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}

	if(pop(dataStack) <= 0){
		cmdbuf->ip += 2;
	}

	return;
}

// Example code:    BRS FOO BAR BAZ
// Compiled result: BRS FOO SKP1 BAR SKP1 BAZ
void branchsign(){
	//printf("In branchsign()\n");
	if((cmdbuf->size - cmdbuf->ip) < 3){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BRS\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BRS\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}

	intptr_t temp = pop(dataStack);
	// If it's <0 we do nothing and the next word runs
	if(temp == 0){ // Do the second thing
		cmdbuf->ip += 2;
	}else{ // Do the third thing
		cmdbuf->ip += 4;
	}
	return;
}

// Example code:    BR 0 FOO 1 BAR 2 BAZ ELSE ERG
// Compiled result: BR 0 FOO SKP2 1 BAR SKP2 2 BAZ ERG
void branch(){
	//printf("In branch()\n");
	//debug();
	if((cmdbuf->size - cmdbuf->ip) < 4){
		fprintf(stderr,"ERROR: Insufficient branch outcomes for BR\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for BR\n");
		//debug();
		cmdClear(cmdbuf);
		return;
	}

	intptr_t tempval = pop(dataStack);

	// TODO Iterate over condition literals until we either find one that matches or isn't followed immediately by SKP2
	// This should be done by way of a pointer value comparison with skip2
	// It is critical that we do not check whether a possible comparison value is an "ELSE" type marker since there can be a value collision with an actual number.
	// Every third address after BR is a value and we only use those as values.
	// Every third address after the first outcome is a SKP2 or an outcome word (which cannot be SKP2) and that's how we find the implicit ELSE.
	printf("BR not working right now\n");
	assert(0);
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
// Here's how it works:
// Pop the data stack. If it's >0, decrement it and push it to the loop stack.
// Then simply return and let word_next() advance into the following word.
// That word is followed by LOOP, which will handle repetition.
// If the data stack is <=0, skip the loop (like what BR+ would do)
void do_begin(){
	//printf("In doloop()\n");
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for DO\n");
		cmdClear(cmdbuf);
		return;
	}
	if((cmdbuf->size - cmdbuf->ip) < 1){
		fprintf(stderr,"ERROR: Insufficient command operands for DO\n");
		cmdClear(cmdbuf);
		return;
	}

	intptr_t tempval = pop(dataStack);
	if(tempval > 0){
		push(loopStack, tempval-1);
	}else{
		cmdbuf->ip += 1; // Skip the following word, which should be LOOP
	}
	return;
}

// Pop the counter from the return stack.
// If it's 0 go to the next word (as normal).
// If it's >0, decrement it, push it, and re-execute the previous word.
void do_loop(){
	//printf("In loop()\n");
	if(returnStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient return stack operands for DO_LOOP\n");
		cmdClear(cmdbuf);
		return;
	}
	intptr_t tempval = pop(loopStack);
	if(tempval > 0){
		push(loopStack, tempval-1);
		cmdbuf->ip -= 2; // Re-execute the previous word (word_next will increment it again before executing)
	}
	return;
}

void rp_begin(){
	if((cmdbuf->size - cmdbuf->ip) < 1){
		fprintf(stderr,"ERROR: Insufficient command operands for RP\n");
		cmdClear(cmdbuf);
		return;
	}

	// In RP, 1 means keep looping, 0 means stop.
	push(loopStack, 1);
	return;
}

void rp_loop(){
	//printf("in rp_loop()\n");
	if(returnStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient return stack operands for RP_LOOP\n");
		cmdClear(cmdbuf);
		return;
	}
	intptr_t tempval = top(loopStack);
	if(tempval == 1){
		// Keep looping
		cmdbuf->ip -= 2; // Re-execute the previous word (word_next will increment it again before executing)
	}else if(tempval == 0){
		// EX/EX-/EX0/EX+/EXT has been run, making the count 0.
		pop(returnStack);
	}else{
		fprintf(stderr, "ERROR: Unexpected value %ld on return stack in rp_loop()\n",tempval);
		exit(-1);
	}
	return;
}

// First replace the top of loopStack with 0
// Then exit words until we are back at the loop level (rp_loop or do_loop will end the loop)
void loop_exit(){
	//printf("In loop_exit()\n");
	// TODO
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

void skip1(){
	cmdbuf->ip += 1;
	return;
}

void skip2(){
	cmdbuf->ip += 2;
	return;
}

void noop(){
	return;
}

void _undefined(){
	printf("Error: Undefined word %s called during execution\n", current_codeword->name);
	// TODO clean return to prompt
	exit(0);
}

void pushLiteral(){
	intptr_t operand = (intptr_t) cmdbuf->array[cmdbuf->ip+1];
	printf("Pushing %ld\n",operand);
	push(dataStack, operand);
	cmdbuf->ip++;
	return;
}

// current_codeword->data should point to a variable struct
void pushVar(){
	variable * var = (variable *)current_codeword->data;
	intptr_t val = var->value;
	push(dataStack, val);
}

// Declare the existence of a named variable
// The data field of the current codeword should contain the address of the desired (char *) name of the variable
// We initialize variables to 0. Not sure whether original DSSP did this.
// FIXME: Variables should be placed in subdicts, not separately. They should share the same namespace as words.
void declareVar(){
	char * varname = (char *) current_codeword->data;

	// See if it is a core word
	 if(coreSearch(varname, vocab)){
		fprintf(stderr,"ERROR: Cannot name variable %s. Name conflict with core dictionary word.\n",varname);
		cmdClear(cmdbuf);
		return;
	}

	// Make sure the name isn't already used in vocab->grow
	int used = growSearch(varname, vocab);
	if(1== used){
		fprintf(stderr,"ERROR: Cannot name variable %s. Name conflict with user dictionary word.\n",varname);
		cmdClear(cmdbuf);
		return;
	}else if(2 == used){
		fprintf(stderr,"ERROR: Cannot name variable %s. Name conflict with prior variable.\n",varname);
		cmdClear(cmdbuf);
		return;
	}

	// No problems. Declare the var.
	variable * tempVar = malloc(sizeof(variable));
	tempVar->name = malloc(1+strlen(varname));
	strcpy(tempVar->name, varname);
	tempVar->value = 0;
	tempVar->next = vocab->grow->varlist;
	vocab->grow->varlist = tempVar;
	printf("Declared variable %s\n",varname);
}

// Assign top of stack to a variable
// The data field of the current codeword should contain the address of the correct variable struct
void assignVar(){
	if(dataStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient data operands for !\n");
		cmdClear(cmdbuf);
		return;
	}

	((variable *)(current_codeword->data))->value = pop(dataStack);
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

// Lists all undefined words
void inventoryUndefined(){
	undefined_word_t * temp = vocab->undefined;
	while(temp != NULL){
		//printf("Traversal sees %s\n",temp->name);
		printf("%s\n",temp->name);
		temp = temp->next;
	}
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
