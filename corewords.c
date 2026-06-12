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

#include <stdlib.h>
#include <string.h>

#include "corewords.h"
#include "dict.h"
#include "stack.h"
#include "cmdbuf.h"
#include "util.h"

#define FETCH_TOP_IND        int top_ind = dataStack->top;
#define TOP_IND              (top_ind)
#define TOP_FROM_IND         (dataStack->array[top_ind])
#define STACKEMPTY           (top_ind == -1)

#define FETCH_LOOP_TOP_IND   int loop_top_ind = loopStack->top;
#define LOOP_STACKEMPTY      (loop_top_ind == -1)
#define FETCH_LOOP_TOP_PTR   intptr_t * loop_top_ptr = &loopStack->array[loop_top_ind];
#define DEREF_LOOP_TOP_PTR   (*loop_top_ptr)

extern codeword_t * current_codeword;
extern stack * returnStack;
extern stack * loopStack;
extern dict * vocab;
extern int abort_requested;

void push_zero(){
	push(dataStack, 0);
}

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
	FETCH_TOP_IND
	intptr_t temp_data;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for +\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_data = pop(dataStack);
	dataStack->array[dataStack->top] += temp_data;
	return;
}

void multiply(){
	FETCH_TOP_IND
	intptr_t temp1;
	intptr_t temp2;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for *\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp1 = pop(dataStack);
	temp2 = top(dataStack);
	dataStack->array[dataStack->top] = temp1 * temp2;
	return;
}

void minus(){
	FETCH_TOP_IND
	intptr_t temp;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for -\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp = pop(dataStack);
	dataStack->array[dataStack->top] -= temp;
	return;
}

void divide(){
	FETCH_TOP_IND
	intptr_t divisor;
	intptr_t dividend;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for /\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	intptr_t * temp_stack_arr = dataStack->array;

	divisor = temp_stack_arr[TOP_IND];
	dividend = temp_stack_arr[TOP_IND-1];
	if(divisor == 0){
		pop(dataStack);
		pop(dataStack);
		fprintf(stderr,"ERR: Division by zero\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_stack_arr[dataStack->top-1] = dividend / divisor;
	temp_stack_arr[dataStack->top] = dividend % divisor;

	return;
}

void negate(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for NEG\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND = -TOP_FROM_IND;
	return;
}

void absval(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for ABS\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	} else if(TOP_FROM_IND < 0) {
		TOP_FROM_IND = -TOP_FROM_IND;
	}
	return;
}

void plus1(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 1+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND += 1;
	return;
}

void plus2(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 2+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND += 2;
	return;
}

void plus3(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 3+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND += 3;
	return;
}

void plus4(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 4+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND += 4;
	return;
}

void minus1(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 1-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND -= 1;
	return;
}

void minus2(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 2-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND -= 2;
	return;
}

void minus3(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 3-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND -= 3;
	return;
}

void minus4(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for 4-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND -= 4;
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
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for IF+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	if(pop(dataStack) <= 0){
		cmdbuf->ip += 1; // Skip the first thing
	}

	return;
}

void ifzero(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for IF0\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	if(pop(dataStack) != 0){
		cmdbuf->ip += 1; // Skip the first thing
	}

	return;
}

void ifminus(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for IF-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
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
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for BR-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
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
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for BR0\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
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
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for BR+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
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
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for BRS\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	intptr_t temp = pop(dataStack);
	// If it's <0 we do nothing and the next word runs
	if(temp == 0){ // Do the second thing
		cmdbuf->ip += 2;
	}else if(temp>0){ // Do the third thing
		cmdbuf->ip += 4;
	}
	return;
}

// Example code:    BR 0 FOO 1 BAR 2 BAZ ELSE ERG
// Compiled result: BR 0 FOO SKP2 1 BAR SKP2 2 BAZ SKP1 ERG
void branch(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for BR\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	intptr_t tempval = pop(dataStack);
	// Iterate over condition literals until we either find one that matches or is preceded by SKP1 instead of BR/SKP2
	// Once we find the correct word to execute (following the matched literal), set cmdbuf->ip to the index preceding it.
	intptr_t offset = 0;
	while(1){
		//printf("offset: %d\n",offset);
		// Check for end of BR-ELSE
		if(cmdbuf->array[cmdbuf->ip + offset]->xt == skip1){
			//printf("Reached end of loop with no match.");
			cmdbuf->ip += offset; // Now points to SKP1, will be incremented again by word_next()
			break;
		}
		offset += 1;
		// Check for literal match
		if((intptr_t) cmdbuf->array[cmdbuf->ip + offset] == tempval){
			//printf("Found match at offset %d\n",offset);
			cmdbuf->ip += offset; // Now points to literal, will be incremented again by word_next()
			break;
		}
		offset += 2;
	}
	return;
}


void equality(){
	FETCH_TOP_IND
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for =\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t a = pop(dataStack);
	intptr_t b = pop(dataStack);
	if(b == a){
		push(dataStack,1);
	}else{
		push(dataStack,0);
	}
	return;
}

void greaterthan(){
	FETCH_TOP_IND
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for >\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t a = pop(dataStack);
	intptr_t b = pop(dataStack);
	if(b > a){
		push(dataStack,1);
	}else{
		push(dataStack,0);
	}
	return;
}

void lessthan(){
	FETCH_TOP_IND
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for <\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t a = pop(dataStack);
	intptr_t b = pop(dataStack);
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
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for DO\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
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
	FETCH_LOOP_TOP_IND
	if(LOOP_STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient loop stack operands for DO_LOOP\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	FETCH_LOOP_TOP_PTR
	intptr_t tempval = DEREF_LOOP_TOP_PTR;
	if(tempval > 0){
		DEREF_LOOP_TOP_PTR -= 1;
		cmdbuf->ip -= 2; // Re-execute the previous word (word_next will increment it again before executing)
	}else{
		pop(loopStack);
	}
	return;
}

void rp_begin(){
	// In RP, 1 means keep looping, 0 means stop.
	push(loopStack, 1);
	return;
}

void rp_loop(){
	FETCH_LOOP_TOP_IND
	if(LOOP_STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient loop stack operands for RP_LOOP\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	FETCH_LOOP_TOP_PTR
	intptr_t tempval = DEREF_LOOP_TOP_PTR;
	if(tempval == 1){
		// Keep looping
		cmdbuf->ip -= 2; // Re-execute the previous word (word_next will increment it again before executing)
	}else if(tempval == 0){
		// EX/EX-/EX0/EX+/EXT has been run, making the count 0.
		pop(loopStack);
	}else{
		fprintf(stderr, "ERR: Unexpected value %ld on return stack in rp_loop()\n",tempval);
		exit(-1); // This is not an error we should recover from as it indicates a problem in libreDSSP itself
	}
	return;
}

// First replace the top of loopStack with 0
// Then exit words until we are back at the loop level (rp_loop or do_loop will end the loop)
void loop_exit(){
	// Make sure we really are in at least one loop
	if(loopStack->top == -1){
		printf("ERR: EX called outside of any loop\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	// So that the loop repeating word won't repeat the loop
	loopStack->array[loopStack->top] = 0;

	// Back out to the looping words
	do{
		cmdbuf->size = (int) pop(returnStack);
		cmdbuf->array = (codeword_t **) pop(returnStack);
		cmdbuf->ip = (int) pop(returnStack);
	}while((cmdbuf->array[cmdbuf->ip+1]->xt != rp_loop) && (cmdbuf->array[cmdbuf->ip+1]->xt != do_loop));
	return;
}

// Minus conditional version of loop_exit
void loop_exit_minus(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for EX-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	if(pop(dataStack) < 0){
		// This is not how most of our commands work but in this case it's fine and saves some memory.
		loop_exit();
	}

	return;
}

// Zero conditional version of loop_exit
void loop_exit_zero(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for EX0\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	if(pop(dataStack) == 0){
		// This is not how most of our commands work but in this case it's fine and saves some memory.
		loop_exit();
	}

	return;
}

// Plus conditional version of loop_exit
void loop_exit_plus(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for EX+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	if(pop(dataStack) > 0){
		// This is not how most of our commands work but in this case it's fine and saves some memory.
		loop_exit();
	}

	return;
}

// Pop value from stack. Attempt to exit that many loops.
// If there aren't that many loops to exit, abort execution.
void loop_exit_nested(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for EXT\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	intptr_t depth = pop(dataStack);
	for( ; depth>0; depth--){
		//printf("Acending. Current depth is %ld\n",depth);
		// Make sure there is a layer to exit
		if(loopStack->top == -1){
			printf("ERR: EXT ran out of loops to exit\n");
			abortExecution();
			cmdbuf->ip = -1; // word_next increments this!
			return;
		}

		// Exit one layer
		do{
			//printf("Exiting one loop...\n");
			cmdbuf->size = (int) pop(returnStack);
			cmdbuf->array = (codeword_t **) pop(returnStack);
			cmdbuf->ip = (int) pop(returnStack);
		}while((cmdbuf->array[cmdbuf->ip+1]->xt != rp_loop) && (cmdbuf->array[cmdbuf->ip+1]->xt != do_loop));
		//printf("Done\n");
		// If there are more layers to exit, we need to entirely forget we were ever in the one we just left.
		if(depth>1){
			//printf("Popping loopstack\n");
			pop(loopStack);
		}
	}
	// We always leave one layer on the loopStack! do_loop/rp_loop require it.
	// So that the loop repeating word won't repeat the loop
	loopStack->array[loopStack->top] = 0;
	//printf("Loopstack top is %d\n",loopStack->top);

	return;
}

// Stack manipulation
void exch2(){
	FETCH_TOP_IND
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for E2\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t temp = dataStack->array[TOP_IND];
	dataStack->array[TOP_IND] = dataStack->array[TOP_IND - 1];
	dataStack->array[TOP_IND - 1] = temp;
	return;
}
void exch3(){
	FETCH_TOP_IND
	if(TOP_IND <= 1){
		fprintf(stderr,"ERR: Insufficient operands for E3\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t temp = dataStack->array[TOP_IND];
	dataStack->array[TOP_IND] = dataStack->array[TOP_IND - 2];
	dataStack->array[TOP_IND - 2] = temp;
	return;
}
void exch4(){
	FETCH_TOP_IND
	if(TOP_IND <= 2){
		fprintf(stderr,"ERR: Insufficient operands for E4\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t temp = dataStack->array[TOP_IND];
	dataStack->array[TOP_IND] = dataStack->array[TOP_IND - 3];
	dataStack->array[TOP_IND - 3] = temp;
	return;
}
void exchdepth(){
	FETCH_TOP_IND
	if(TOP_IND <= 0){ // There need to be two or more operands
		fprintf(stderr,"ERR: Insufficient operands for ET\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t depth = pop(dataStack);
	if (depth == 1) return;
	if(dataStack->top <= depth-2){
		fprintf(stderr,"ERR: Insufficient operands for ET\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERR: Can only exchange from positive depth\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t temp = top(dataStack);
	dataStack->array[dataStack->top] = dataStack->array[dataStack->top - (depth - 1)];
	dataStack->array[dataStack->top - (depth - 1)] = temp;
	return;
}

void copy(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for C\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	push(dataStack,top(dataStack));
	return;
}

void copy2(){
	FETCH_TOP_IND
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for C2\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	push(dataStack, dataStack->array[TOP_IND - 1]);
	return;
}
void copy3(){
	FETCH_TOP_IND
	if(TOP_IND <= 1){
		fprintf(stderr,"ERR: Insufficient operands for C3\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	push(dataStack, dataStack->array[TOP_IND - 2]);
	return;
}
void copy4(){
	FETCH_TOP_IND
	if(TOP_IND <= 2){
		fprintf(stderr,"ERR: Insufficient operands for C4\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	push(dataStack, dataStack->array[TOP_IND - 3]);
	return;
}
void copydepth(){
	FETCH_TOP_IND
	if(TOP_IND <= 0){ // There need to be two or more operands
		fprintf(stderr,"ERR: Insufficient operands for CT\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	intptr_t depth = pop(dataStack);
	if((TOP_IND-1) <= depth-2){
		fprintf(stderr,"ERR: Insufficient operands for CT\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	if(depth <= 0){
		fprintf(stderr,"ERR: Can only copy from positive depth\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	push(dataStack, dataStack->array[dataStack->top - (depth - 1)]);
	return;
}

void drop(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for D\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
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
	printf("ERR: Undefined word %s called during execution\n", current_codeword->name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

// Next cell should contain a constant literal
void pushLiteral(){
	intptr_t operand = (intptr_t) cmdbuf->array[cmdbuf->ip+1];
	//printf("Pushing %ld\n",operand);
	push(dataStack, operand);
	cmdbuf->ip++; // Skip data cell
	return;
}

// Push the current value of a variable
// The next cell should contain the address of the correct variable struct
void pushVar(){
	push(dataStack, ((variable_t *)(cmdbuf->array[cmdbuf->ip+1]))->value);
	cmdbuf->ip++; // Skip data cell
	return;
}

// Declare the existence of a named variable
// The next cell should contain the address of the desired (char *) name of the variable
// We initialize variables to 0. Not sure whether original DSSP did this.
void declareVar(){
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(vocab->grow == NULL){
		printf("Error: We are defining a word but no dictionary is selected\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	if(vocab->grow->open == 0){
		printf("Error: We are defining a word in a closed dictionary\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname,vocab);
	if(collision != 0){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}

	// No problems. Declare the var.
	variable_t * tempVar = malloc(sizeof(variable_t));
	tempVar->name = varname; // The parser allocated this buffer and we can keep it
	tempVar->value = 0;
	tempVar->next = vocab->grow->varlist;
	vocab->grow->varlist = tempVar;
	//printf("Declared variable %s\n",varname);
	return;
}

// Assign top of stack to a variable
// The next cell should contain the address of the correct variable struct
void assignVar(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for !\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	cmdbuf->ip++; // Advance to data cell
	((variable_t *)(cmdbuf->array[cmdbuf->ip]))->value = pop(dataStack);
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

// Select a subdict for growth. If it doesn't exist yet, open it.
// The next cell should contain a valid dictionary name (the parser has checked the format)
void growSub(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);

	// We need to check if the subdict exists using findDict(), and if it does we need to check if it is open.
	// If it doesn't exist, we need to create it and open it.
	subdict * tempSub = findDict(name, vocab);

	if(tempSub == NULL){ // We are making a new subdict
		tempSub = newDict(name, vocab);
	}

	vocab->grow = tempSub;
	tempSub->open = 1;
	return;
}

// Shut a subdict, hiding all words defined therein
// The next cell should contain a valid subdict *
void shutSub(){
	cmdbuf->ip++; // Advance to data cell
	subdict * tempSub =  (subdict *)(cmdbuf->array[cmdbuf->ip]);
	tempSub->open = 0;
	return;
}

// Open a subdict, revealing all words defined therein
// The next cell should contain a valid subdict *
void openSub(){
	cmdbuf->ip++; // Advance to data cell
	subdict * tempSub =  (subdict *)(cmdbuf->array[cmdbuf->ip]);
	tempSub->open = 1;
	return;
}

// TODO Confirm that this is the correct behavior
// TODO Support multiple number bases
void termInNum(){
	if(dataStack->top > -1){ // Requires one operand
		intptr_t len = pop(dataStack);
		if(len < 0){
			fprintf(stderr,"ERR: TIN requires non-negative operand\n");
			abortExecution();
			cmdbuf->ip = -1; // word_next increments this!
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
			fprintf(stderr,"ERR: TIN could not read line\n");
			abortExecution();
			cmdbuf->ip = -1; // word_next increments this!
			return;
		}
	}else{
		fprintf(stderr,"ERR: Insufficient operands for TIN\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	return;
}

// TODO Confirm that this is the correct behavior
// TODO Support multiple number bases
void termOutNum(){
	if(dataStack->top > 0){ // Requires two operands
		intptr_t len = pop(dataStack);
		intptr_t num = pop(dataStack);
		int maxlen = snprintf( NULL, 0, "%ld", num );
		char *toPrint = malloc( maxlen + 1 );
		snprintf( toPrint, maxlen + 1, "%ld", num );


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
		fprintf(stderr,"ERR: Insufficient operands for TON\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	return;
}

void termOutString(){
	// Requires two operands. The top of the stack is a count and below that is a pointer to the string.
	if(dataStack->top < 1){
		fprintf(stderr,"ERR: Insufficient stack operands for TOS\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	// Pop the count and pointer from the stack
	intptr_t len = pop(dataStack);
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


void define_all_core(dict * vocab){
	// Most common literals
	defCore("0", push_zero, vocab);
	defCore("1", push_one, vocab);
	defCore("2", push_two, vocab);
	defCore("4", push_four, vocab);
	defCore("8", push_eight, vocab);

	// Arithmetic
	defCore("+", plus, vocab);
	defCore("*", multiply, vocab);
	defCore("-", minus, vocab);
	defCore("/", divide, vocab);
	defCore("NEG", negate, vocab);
	defCore("ABS", absval, vocab);
	defCore("1+", plus1, vocab);
	defCore("2+", plus2, vocab);
	defCore("3+", plus3, vocab);
	defCore("4+", plus4, vocab);
	defCore("1-", minus1, vocab);
	defCore("2-", minus2, vocab);
	defCore("3-", minus3, vocab);
	defCore("4-", minus4, vocab);

	// Display and interpreter
	defCore("BYE", bye, vocab);
	defCore(".", showTop, vocab);
	defCore("..", showStack, vocab);
	defCore("B10", base10, vocab);

	// Conditionals
	defCore("IF+", ifplus, vocab);
	defCore("IF0", ifzero, vocab);
	defCore("IF-", ifminus, vocab);
	defCore("BR-", branchminus, vocab);
	defCore("BR0", branchzero, vocab);
	defCore("BR+", branchplus, vocab);
	defCore("BRS", branchsign, vocab);
	defCore("BR", branch, vocab);
	defCore("=", equality, vocab);
	defCore(">", greaterthan, vocab);
	defCore("<", lessthan, vocab);

	// Looping and flow control
	defCore("DO", do_begin, vocab);
	defCore("DO_LOOP", do_loop, vocab); // Not to be used directly
	defCore("RP", rp_begin, vocab);
	defCore("RP_LOOP", rp_loop, vocab); // Not to be used directly
	defCore("EX", loop_exit, vocab);
	defCore("EX-", loop_exit_minus, vocab);
	defCore("EX0", loop_exit_zero, vocab);
	defCore("EX+", loop_exit_plus, vocab);
	defCore("EXT", loop_exit_nested, vocab);

	// Stack manipulation
	defCore("E2", exch2, vocab);
	defCore("E3", exch3, vocab);
	defCore("E4", exch4, vocab);
	defCore("ET", exchdepth, vocab);
	defCore("C", copy, vocab);
	defCore("C2", copy2, vocab);
	defCore("C3", copy3, vocab);
	defCore("C4", copy4, vocab);
	defCore("CT", copydepth, vocab);
	defCore("D", drop, vocab);
	defCore("DS", dropStack, vocab);

	// Misc
	// TODO for special functions we should just use references instead of the dictionary.
	defCore("PUSHLIT", pushLiteral, vocab);
	//defCore("DOCOLON", word_enter, vocab); // Not to be used directly
	defCore(";S", word_exit, vocab); // Not to be used directly
	defCore("SKP1", skip1, vocab); // Not to be used directly
	defCore("SKP2", skip2, vocab); // Not to be used directly
	defCore("NOP", noop, vocab);
	defCore("VAR", declareVar, vocab);
	defCore("!", assignVar, vocab);
	defCore("PUSHVAR", pushVar, vocab); // Not to be used directly
	defCore("CR", printNewline, vocab);
	defCore("SP", printSpace, vocab);
	defCore("?$", listDicts, vocab);
	defCore("GROW", growSub, vocab);
	defCore("SHUT", shutSub, vocab);
	defCore("USE", openSub, vocab);
	defCore("TIN", termInNum, vocab);
	defCore("TON", termOutNum, vocab);
	defCore("TOS", termOutString, vocab);
	defCore("DEEP", stackDepth, vocab);
	defCore("UNDEF",inventoryUndefined, vocab);
	defCore("WORDS", inventoryWords, vocab); // Borrowed from FORTH. Currently unsure if DSSP had an equivalent.
	// TODO should add words that check word size of the machine (64 or 32 bits) to enable portable DSSP code.
}