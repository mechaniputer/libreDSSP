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
		push(returnStack, (intptr_t) cmdbuf->size);
		// Set branch IP
		cmdbuf->array = (codeword_t **) (outcome->data);
		cmdbuf->ip = -1; // The loop in word_next() will increment this to 0
	}else if(!strcmp(outcome->name, "EX")){
		current_codeword = outcome; // Important for pushLiteral to reference the correct data field
		(*outcome->xt)();
		// Don't change the IP
	}else{ // Literal, print, or core
		current_codeword = outcome; // Important for pushLiteral to reference the correct data field
		(*outcome->xt)();
		cmdbuf->ip += 2;
	}
}

// Similar to branch_helper but for the three-way branch in BRS. Main difference is that the IP increment is 3 instead of 2.
// FIXME The check to see if we are executing EX is hacky and won't work for EXT!
//       I need a better way to alter control flow here
//       I believe the right approach is to use "JUMP" words to skip non-taken branch outcomes.
//       Note that this will increase the size of user defined words.
//       At first these JUMP words will just skip the next intruction, but as a later optimization they can each be set to skip to the end of the branch.
//       With that change, branches will not need to call xt() anymore (we will rely fully on word_next()).
//       Branches will still set the IP forwards once (at most) but won't need to do any follow-up.
//       The next step will be to modify EX to (still clean up the loop counter and then) do what word_exit() does until we find the loop we are in.
//       We can find the loop by exiting words until we are about to execute do_loop or rp_loop (there might be a cleaner way)
//       As long as everything else in the call chain simply returns to word_next() and we run do_loop or rp_loop, it should work.
//       EXT can do the same thing for more layers of loops. Again, if we find our way back to word_next(), it should be fine.
static inline void brs_helper(codeword_t *outcome){
	if(outcome->user == 1){
		// Push IP to return to after branch completes
		push(returnStack, (intptr_t) (cmdbuf->ip)+3);
		push(returnStack, (intptr_t) cmdbuf->array);
		push(returnStack, (intptr_t) cmdbuf->size);
		// Set branch IP
		cmdbuf->array = (codeword_t **) (outcome->data);
		cmdbuf->ip = -1; // The loop in word_next() will increment this to 0
	}else if(!strcmp(outcome->name, "EX")){
		current_codeword = outcome; // Important for pushLiteral to reference the correct data field
		(*outcome->xt)();
		// Don't change the IP since EX just changed it to point to ;S
	}else{ // Literal, print, or core
		current_codeword = outcome; // Important for pushLiteral to reference the correct data field
		(*outcome->xt)();
		cmdbuf->ip += 3;
	}
}

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

	if(pop(dataStack) < 0){ // Do the first thing
		//printf("Doing first thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else{
		//printf("Doing second thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}

	return;
}

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

	if(pop(dataStack) == 0){ // Do the first thing
		//printf("Doing first thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else{
		//printf("Doing second thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}

	return;
}

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

	if(pop(dataStack) > 0){ // Do the first thing
		//printf("Doing first thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else{
		//printf("Doing second thing\n");
		branch_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}

	return;
}

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
	if(temp < 0){ // Do the first thing
		//printf("Doing first thing\n");
		brs_helper(cmdbuf->array[(cmdbuf->ip)+1]);
	}else if(temp == 0){ // Do the second thing
		//printf("Doing second thing\n");
		brs_helper(cmdbuf->array[(cmdbuf->ip)+2]);
	}else{ // Do the third thing
		//printf("Doing third thing\n");
		brs_helper(cmdbuf->array[(cmdbuf->ip)+3]);
	}
	return;
}

// Example of branch syntax:
// BR 0 P0 1 P1 2 P2 ELSE P3
// Minimum operands: 1 condition, 1 outcome, 1 ELSE, 1 more outcome. (total 4)
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
	int tempcondip = cmdbuf->ip+1; // IP of condition we are checking
	codeword_t * tempcond = cmdbuf->array[tempcondip];
	while(strcmp(tempcond->name, "ELSE") && (tempcond->data != tempval)){
		tempcondip += 2;
		tempcond = cmdbuf->array[tempcondip];
	}
	int elseip = tempcondip;
	while(cmdbuf->array[elseip]!= NULL && strcmp(cmdbuf->array[elseip]->name, "ELSE")){
		elseip += 2;
		if(elseip >= cmdbuf->size){
			assert(0);
		}
	}
	assert(!strcmp(cmdbuf->array[elseip]->name, "ELSE"));

	codeword_t * outcome = cmdbuf->array[tempcondip+1];
	if(outcome->user == 1){
		// Push IP to return to after branch completes
		push(returnStack, (intptr_t) elseip+1);
		push(returnStack, (intptr_t) cmdbuf->array);
		push(returnStack, (intptr_t) cmdbuf->size);
		//printf("BR() pushed return IP: %d cmdbuf->array: %p\n", (int) elseip+1, (void*)cmdbuf->array);
		// Set branch IP
		cmdbuf->array = (codeword_t **) (outcome->data);
		cmdbuf->ip = -1; // The loop in word_next() will increment this to 0
	}else{ // Literal, print, or core
		current_codeword = outcome; // Important for pushLiteral to reference the correct data field
		(*outcome->xt)();
		cmdbuf->ip =elseip+2;
	}
	return;
}

// BR won't actually run this word. It mostly exists to prevent user from defining ELSE.
// Should find a more elegant solution later.
// Maybe use the BR data field to indicate the ELSE outcome IP.
// Would still need to reserve the word (add a reserved words mechanism apart from corewords)
void brelse(){
	printf("In brelse(), but this function shouldn't run!\n");
	assert(0);
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
// Pop the data stack. If it's >0, decrement it and push it to the return stack.
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
		push(returnStack, tempval-1);
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
	intptr_t tempval = pop(returnStack);
	if(tempval > 0){
		push(returnStack, tempval-1);
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
	push(returnStack, 1);
	return;
}

void rp_loop(){
	//printf("in rp_loop()\n");
	if(returnStack->top < 0){
		fprintf(stderr,"ERROR: Insufficient return stack operands for RP_LOOP\n");
		cmdClear(cmdbuf);
		return;
	}
	intptr_t tempval = top(returnStack);
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

// First replace the top of the return stack with 0
// Then exit the current word by skipping ahead to word_exit().
void loop_exit(){
	//printf("In loop_exit()\n");
	// The return stack will have 3 values on top of the one we actually need.
	intptr_t tempval = returnStack->array[returnStack->top - 3];
	if(tempval <= 0){
		fprintf(stderr, "ERROR: Unexpected value %ld on return stack in loop_exit()\n",tempval);
		exit(-1);
	}else{
		returnStack->array[returnStack->top - 3] = 0;
	}
	//debug();
	cmdbuf->ip = cmdbuf->size -2; // Will be incremented again by word_next()
	//printf("EX jumping to command: %s\n", cmdbuf->array[cmdbuf->ip + 1]->name);
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

void noop(){
	return;
}

void _undefined(){
	printf("Error: Undefined word %s called during execution\n", current_codeword->name);
	// TODO clean return to prompt
	exit(0);
}

void pushLiteral(){
	//printf("Pushing %ld\n",current_codeword->data);
	push(dataStack, current_codeword->data);
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
