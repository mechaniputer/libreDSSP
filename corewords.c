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
#include <termios.h> // For echo disable in termReadByte()
#include <unistd.h>  // For STDIN_FILENO in termReadByte()

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


// Area for core word function bodies

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

void sign(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for SGN\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	} else if(TOP_FROM_IND < 0) {
		TOP_FROM_IND = -1;
	}else if(TOP_FROM_IND > 0) {
		TOP_FROM_IND = 1;
	}
	// 0 is already 0
	return;
}

void max(){
	FETCH_TOP_IND
	intptr_t temp_data;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for MAX\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_data = pop(dataStack);
	top_ind -=1;
	if(temp_data > TOP_FROM_IND){
		TOP_FROM_IND = temp_data;
	}
	return;
}

void min(){
	FETCH_TOP_IND
	intptr_t temp_data;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for MIN\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_data = pop(dataStack);
	top_ind -=1;
	if(temp_data < TOP_FROM_IND){
		TOP_FROM_IND = temp_data;
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

void top0(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for T0\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND = 0;
	return;
}

void top1(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for T1\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND = 1;
	return;
}

void shift_left(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for SHL\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND <<=1;
	return;
}

void shift_right(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for SHR\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND >>=1;
	return;
}

void shift_val(){
	FETCH_TOP_IND
	intptr_t temp_data;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for SHT\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_data = pop(dataStack);
	top_ind -=1;

	if(temp_data > 0){
		TOP_FROM_IND <<= temp_data;
	}else if(temp_data < 0){
		TOP_FROM_IND >>= ((~temp_data)+1);
	}

	return;
}

void not(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for NOT\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND = !TOP_FROM_IND;
	return;
}

void bit_inv(){
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient operands for INV\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	TOP_FROM_IND = ~TOP_FROM_IND;
	return;
}

void bit_and(){
	FETCH_TOP_IND
	intptr_t temp_data;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for &\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_data = pop(dataStack);
	dataStack->array[dataStack->top] &= temp_data;
	return;
}

void bit_or(){
	FETCH_TOP_IND
	intptr_t temp_data;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for &0\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_data = pop(dataStack);
	dataStack->array[dataStack->top] |= temp_data;
	return;
}

void bit_xor(){
	FETCH_TOP_IND
	intptr_t temp_data;
	if(TOP_IND <= 0){
		fprintf(stderr,"ERR: Insufficient operands for '+'\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	temp_data = pop(dataStack);
	dataStack->array[dataStack->top] ^= temp_data;
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

void delete_name(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	deleteName(name);
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
		printf("Warning: no subdictionary selected for expansion\n");
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

// Select a subdict for modification. If it doesn't exist yet, open it.
// The next cell should contain a valid dictionary name (the parser has checked the format)
void growSub(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);

	// We need to check if the subdict exists using findDict(), and if it does we need to check if it is open.
	// If it doesn't exist, we need to create it and open it.
	subdict * tempSub = findDict(name);

	if(tempSub == NULL){ // We are making a new subdict
		tempSub = newDict(name);
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

// Read byte without echo
void termReadByte(){
	// FIXME Need ctrl+c sig handler to fix the terminal

	// Disable echo, clear ICANON
	struct termios orig, raw;
	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ICANON | ECHO);
	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &raw);

	// Read byte
	int ch = getchar();

	// Enable echo, set ICANON
	tcsetattr(STDIN_FILENO, TCSANOW, &orig);

	if (ch != EOF) {
		push(dataStack, (intptr_t) ch);
	}else{
		fprintf(stderr, "ERR: TRB could not read from stdin\n");
		abortExecution();
		cmdbuf->ip = -1;
		return;
	}
	return;
}

// Read byte with echo
void termInByte(){
	// FIXME Need ctrl+c sig handler to fix the terminal

	// Clear ICANON
	struct termios orig, raw;
	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ICANON);
	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &raw);

	// Read byte
	int ch = getchar();

	// Set ICANON
	tcsetattr(STDIN_FILENO, TCSANOW, &orig);

	if (ch != EOF) {
		push(dataStack, (intptr_t) ch);
	}else{
		fprintf(stderr, "ERR: TIB could not read from stdin\n");
		abortExecution();
		cmdbuf->ip = -1;
		return;
	}
	return;
}

// TODO Support multiple number bases
void termInNum(){
	char buf[70]; // Longest number should be 64-bit binary plus terminator. 65 should be enough.
	if (fgets(buf, sizeof(buf), stdin) != NULL) {
		char *endptr;
		intptr_t num = (intptr_t)strtol(buf, &endptr, 10);
		push(dataStack, num);
	}else{
		fprintf(stderr, "ERR: TIN could not read from stdin\n");
		abortExecution();
		cmdbuf->ip = -1;
		return;
	}
	return;
}

void termOutByte(){
	// Requires one operands.
	if(dataStack->top < 0){
		fprintf(stderr,"ERR: Insufficient stack operands for TOB\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	putchar((char) pop(dataStack));
	return;
}

// TODO Support multiple number bases
void termOutNum(){
	if (dataStack->top >= 1) { // Requires two operands
		intptr_t len = pop(dataStack);
		intptr_t num = pop(dataStack);

		if (len <= 0) {
			return;
		}
		char buf[70]; // Longest number should be 64-bit binary plus terminator. 65 should be enough.
		int maxlen = snprintf(buf, sizeof(buf), "%ld", (long)num);

		if (maxlen > len) {
			// Truncation case
			printf("%s", buf + (maxlen - len));
		}else{
			// Padding case (including no padding)
			int spaces = (int)(len - maxlen);
			for (int i = 0; i < spaces; i++) {
				putchar(' ');
			}
			printf("%s", buf);
		}

	}else{
		fprintf(stderr,"ERR: Insufficient operands for TON\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	return;
}

void termInString(){
	// FIXME Need ctrl+c sig handler to fix the terminal

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

	// Read up to the specified number of characters from the string
	for(int i=0; i<len; i++){

		// Clear ICANON
		struct termios orig, raw;
		tcgetattr(STDIN_FILENO, &orig);
		raw = orig;
		raw.c_lflag &= ~(ICANON);
		raw.c_cc[VMIN] = 1;
		raw.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &raw);

		str[i] = getchar();

		// Set ICANON
		tcsetattr(STDIN_FILENO, TCSANOW, &orig);

		// If newline or EOF, break
		if(str[i] == '\n' || str[i] == EOF){
			str[i] = '\0'; // Null terminate the string
			break;
		}
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
	// Print up to the specified number of characters from the string
	for(int i=0; i<len; i++){
		if(str[i] == '\0' || str[i] == EOF) break;
		printf("%c", str[i]);
	}
	return;
}

void bell(){
	putchar('\a');
	return;
}

void stackDepth(){
	push(dataStack, (dataStack->top)+1);
	return;
}

// Lists all undefined words
void inventoryUndefined(){
	undefined_ref_t * temp = vocab->undefined;
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
				tempWord = tempWord->next;
			}
			printf("\n");
		}
		tempSub = tempSub->next;
	}
	return;
}

// Prints definition of a name (all occurrences in all open dictionaries)
// The next cell should contain a valid name (the parser has checked the format)
void seeName(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);

	subdict * tempSub = vocab->sub;
	codeword_t * tempWord;
	variable_t * tempVar;
	while(tempSub != NULL){

		// First check for a word
		tempWord = tempSub->wordlist;
		while(tempWord != NULL){
			if(!strcmp(name, tempWord->name)){
				printf("Subdict: %s (%s)\n",tempSub->name, (tempSub->open ? "OPEN" : "CLOSED"));
				printf("  %s\n",tempWord->text);
				break; // One occurrence max per subdict
			}
			tempWord = tempWord->next;
		}

		// Next check for a var
		tempVar = tempSub->varlist;
		while(tempVar != NULL){
			// TODO this will need to be modified to print vectors and arrays
			if(!strcmp(name, tempVar->name)){
				printf("Subdict: %s (%s)\n",tempSub->name, (tempSub->open ? "OPEN" : "CLOSED"));
				printf("  %s: %ld\n",tempVar->name, tempVar->value);
				break; // One occurrence max per subdict
			}
			tempVar = tempVar->next;
		}
		tempSub = tempSub->next;
	}
	return;
}

void inventoryVars(){
	subdict * tempSub = vocab->sub;
	variable_t * tempVar;
	while(tempSub != NULL){
		if(tempSub->open){
			printf("Subdict: %s\n",tempSub->name);
			tempVar = tempSub->varlist;
			while(tempVar != NULL){
				printf("  %s",tempVar->name);
				tempVar = tempVar->next;
			}
			printf("\n");
		}
		tempSub = tempSub->next;
	}
	return;
}

int declare_var_dict_check(){
	if(vocab->grow == NULL){
		printf("Error: We are defining a word but no dictionary is selected\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return 1;
	}
	if(vocab->grow->open == 0){
		printf("Error: We are defining a word in a closed dictionary\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return 1;
	}
	return 0;
}

// Sets up everything except size and cw_pushVar.xt
variable_t * help_get_new_var(char * varname){
	variable_t * tempVar = malloc(sizeof(variable_t));
	tempVar->name = varname; // The parser allocated this buffer and we can keep it
	tempVar->value = 0;
	tempVar->next = vocab->grow->varlist;
	vocab->grow->varlist = tempVar;

	// per-variable unique _var8_ref op
	tempVar->cw_pushVar.name = varname;
	tempVar->cw_pushVar.data = (intptr_t) tempVar;
	tempVar->cw_pushVar.next = NULL;
	tempVar->cw_pushVar.text = NULL;
	tempVar->cw_pushVar.size = 0; // FIXME maybe we should co-opt this as the variable size field?
	return tempVar;
}

void declare_var1(){
	variable_t * tempVar;
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(declare_var_dict_check()) return;

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname);
	if(collision == 1){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}else if(collision == 2){
		// The var already exists, so we zero it
		tempVar = varSearch(varname);
		tempVar->value = 0;
		return;
	}
	// 0: no collision / 3: undef exists

	// No problems. Declare the var.
	tempVar = help_get_new_var(varname);
	tempVar->size = 1;
	tempVar->cw_pushVar.xt = _var1_ref;

	if(collision == 3) resolve_undefined_ref(varname, /*dict entry*/ NULL, /*isVar*/ 1, /*var*/ tempVar);
	//printf("Declared VAR1 %s\n",varname);
	return;
}

void declare_var1u(){
	variable_t * tempVar;
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(declare_var_dict_check()) return;

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname);
	if(collision == 1){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}else if(collision == 2){
		// The var already exists, so we zero it
		tempVar = varSearch(varname);
		tempVar->value = 0;
		return;
	}
	// 0: no collision / 3: undef exists

	// No problems. Declare the var.
	tempVar = help_get_new_var(varname);
	tempVar->size = 1;
	tempVar->cw_pushVar.xt = _var1u_ref;

	if(collision == 3) resolve_undefined_ref(varname, /*dict entry*/ NULL, /*isVar*/ 1, /*var*/ tempVar);
	//printf("Declared VAR1 %s\n",varname);
	return;
}

void declare_var2(){
	variable_t * tempVar;
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(declare_var_dict_check()) return;

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname);
	if(collision == 1){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}else if(collision == 2){
		// The var already exists, so we zero it
		tempVar = varSearch(varname);
		tempVar->value = 0;
		return;
	}
	// 0: no collision / 3: undef exists

	// No problems. Declare the var.
	tempVar = help_get_new_var(varname);
	tempVar->size = 2;
	tempVar->cw_pushVar.xt = _var2_ref;

	if(collision == 3) resolve_undefined_ref(varname, /*dict entry*/ NULL, /*isVar*/ 1, /*var*/ tempVar);
	//printf("Declared VAR1 %s\n",varname);
	return;
}

void declare_var2u(){
	variable_t * tempVar;
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(declare_var_dict_check()) return;

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname);
	if(collision == 1){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}else if(collision == 2){
		// The var already exists, so we zero it
		tempVar = varSearch(varname);
		tempVar->value = 0;
		return;
	}
	// 0: no collision / 3: undef exists

	// No problems. Declare the var.
	tempVar = help_get_new_var(varname);
	tempVar->size = 2;
	tempVar->cw_pushVar.xt = _var2u_ref;

	if(collision == 3) resolve_undefined_ref(varname, /*dict entry*/ NULL, /*isVar*/ 1, /*var*/ tempVar);
	//printf("Declared VAR1 %s\n",varname);
	return;
}

void declare_var4(){
	variable_t * tempVar;
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(declare_var_dict_check()) return;

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname);
	if(collision == 1){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}else if(collision == 2){
		// The var already exists, so we zero it
		tempVar = varSearch(varname);
		tempVar->value = 0;
		return;
	}
	// 0: no collision / 3: undef exists

	// No problems. Declare the var.
	tempVar = help_get_new_var(varname);
	tempVar->size = 4;
	tempVar->cw_pushVar.xt = _var4_ref;

	if(collision == 3) resolve_undefined_ref(varname, /*dict entry*/ NULL, /*isVar*/ 1, /*var*/ tempVar);
	//printf("Declared VAR1 %s\n",varname);
	return;
}

void declare_var4u(){
	variable_t * tempVar;
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(declare_var_dict_check()) return;

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname);
	if(collision == 1){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}else if(collision == 2){
		// The var already exists, so we zero it
		tempVar = varSearch(varname);
		tempVar->value = 0;
		return;
	}
	// 0: no collision / 3: undef exists

	// No problems. Declare the var.
	tempVar = help_get_new_var(varname);
	tempVar->size = 4;
	tempVar->cw_pushVar.xt = _var4u_ref;

	if(collision == 3) resolve_undefined_ref(varname, /*dict entry*/ NULL, /*isVar*/ 1, /*var*/ tempVar);
	//printf("Declared VAR1 %s\n",varname);
	return;
}

// Declare the existence of a named variable
// The next cell should contain the address of the desired (char *) name of the variable
// We initialize variables to 0.
void declare_var8(){
	variable_t * tempVar;
	cmdbuf->ip++; // Advance to data cell
	char * varname = (char *) cmdbuf->array[cmdbuf->ip];
	// The parser has already ensured that the name is valid and null-terminated

	// If no dictionary is selected to declare the VAR, abort!
	if(declare_var_dict_check()) return;

	// If there is a name collision in the current subdictionary, abort!
	int collision = collisionSearch(varname);
	if(collision == 1){
		printf("Error: name %s is already used\n",varname);
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}else if(collision == 2){
		// The var already exists, so we zero it
		tempVar = varSearch(varname);
		tempVar->value = 0;
		return;
	}
	// 0: no collision / 3: undef exists

	// No problems. Declare the var.
	tempVar = help_get_new_var(varname);
	tempVar->size = 8;
	tempVar->cw_pushVar.xt = _var8_ref;

	if(collision == 3) resolve_undefined_ref(varname, /*dict entry*/ NULL, /*isVar*/ 1, /*var*/ tempVar);
	//printf("Declared VAR1 %s\n",varname);
	return;
}

void define_all_core(){
	// Most common literals
	defCore("0", push_zero);
	defCore("1", push_one);
	defCore("2", push_two);
	defCore("4", push_four);
	defCore("8", push_eight);

	// Stack arithmetic
	defCore("+", plus);
	defCore("*", multiply);
	defCore("-", minus);
	defCore("/", divide);
	defCore("NEG", negate);
	defCore("ABS", absval);
	defCore("SGN", sign);
	defCore("MAX", max);
	defCore("MIN", min);
	defCore("1+", plus1);
	defCore("2+", plus2);
	defCore("3+", plus3);
	defCore("4+", plus4);
	defCore("1-", minus1);
	defCore("2-", minus2);
	defCore("3-", minus3);
	defCore("4-", minus4);
	defCore("T0", top0);
	defCore("T1", top1);
	defCore("SHL", shift_left);
	defCore("SHR", shift_right);
	defCore("SHT", shift_val);
	defCore("NOT", not);
	defCore("INV", bit_inv);
	defCore("&", bit_and);
	defCore("&0", bit_or);
	defCore("'+'", bit_xor);

	// Display and interpreter
	defCore("BYE", bye);
	defCore(".", showTop);
	defCore("..", showStack);
	defCore("B10", base10);

	// Conditionals
	defCore("IF+", ifplus);
	defCore("IF0", ifzero);
	defCore("IF-", ifminus);
	defCore("BR-", branchminus);
	defCore("BR0", branchzero);
	defCore("BR+", branchplus);
	defCore("BRS", branchsign);
	defCore("BR", branch);
	defCore("=", equality);
	defCore(">", greaterthan);
	defCore("<", lessthan);

	// Looping and flow control
	defCore("DO", do_begin);
	defCore("DO_LOOP", do_loop); // Not to be used directly
	defCore("RP", rp_begin);
	defCore("RP_LOOP", rp_loop); // Not to be used directly
	defCore("EX", loop_exit);
	defCore("EX-", loop_exit_minus);
	defCore("EX0", loop_exit_zero);
	defCore("EX+", loop_exit_plus);
	defCore("EXT", loop_exit_nested);

	// Stack manipulation
	defCore("E2", exch2);
	defCore("E3", exch3);
	defCore("E4", exch4);
	defCore("ET", exchdepth);
	defCore("C", copy);
	defCore("C2", copy2);
	defCore("C3", copy3);
	defCore("C4", copy4);
	defCore("CT", copydepth);
	defCore("D", drop);
	defCore("DS", dropStack);

	// Misc
	// TODO for special functions we should just use references instead of the dictionary.
	defCore("PUSHLIT", pushLiteral);
	defCore(";S", word_exit); // Not to be used directly
	defCore("SKP1", skip1); // Not to be used directly
	defCore("SKP2", skip2); // Not to be used directly
	defCore("NOP", noop);
	defCore("VAR1", declare_var1);
	defCore("VAR1U", declare_var1u);
	defCore("VAR2", declare_var2);
	defCore("VAR2U", declare_var2u);
	defCore("VAR4", declare_var4);
	defCore("VAR4U", declare_var4u);
	defCore("VAR8", declare_var8);
	defCore("VAR", declare_var8); // FIXME On 32-bit this should be declare_var4
	defCore("CR", printNewline);
	defCore("SP", printSpace);
	defCore("?$", listDicts);
	defCore("GROW", growSub);
	defCore("SHUT", shutSub);
	defCore("USE", openSub);
	defCore("DEL", delete_name);
	defCore("TRB", termReadByte);
	defCore("TIB", termInByte);
	defCore("TIN", termInNum);
	defCore("TOB", termOutByte);
	defCore("TON", termOutNum);
	defCore("TIS", termInString);
	defCore("TOS", termOutString);
	defCore("BELL", bell);
	defCore("DEEP", stackDepth);
	defCore("UNDEF",inventoryUndefined);
	defCore("WORDS", inventoryWords); // Borrowed from Forth.
	defCore("VARS", inventoryVars); // No precedent
	defCore("SEE", seeName); // Borrowed from Forth. Also examines variables.
	// TODO should add words that check word size of the machine (64 or 32 bits) to enable portable DSSP code.
}


//***************************************************************************
// Section for core words that should not have searchable dictionary entries:
//***************************************************************************


// Codeword is allocated as part of specific undefined_ref_t
void _undefined_ref(){
	printf("ERR: Undefined name %s referenced during execution\n", current_codeword->name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

// Codeword is allocated as part of specific variable_t
void _var1_ref(){
	variable_t *v = (variable_t*) current_codeword->data;
	push(dataStack, (int8_t) v->value);
	return;
}

// Codeword is allocated as part of specific variable_t
void _var1u_ref(){
	variable_t *v = (variable_t*) current_codeword->data;
	push(dataStack, (uint8_t) v->value);
	return;
}

// Codeword is allocated as part of specific variable_t
void _var2_ref(){
	variable_t *v = (variable_t*) current_codeword->data;
	push(dataStack, (int16_t) v->value);
	return;
}

// Codeword is allocated as part of specific variable_t
void _var2u_ref(){
	variable_t *v = (variable_t*) current_codeword->data;
	push(dataStack, (uint16_t) v->value);
	return;
}

// Codeword is allocated as part of specific variable_t
void _var4_ref(){
	variable_t *v = (variable_t*) current_codeword->data;
	push(dataStack, (int32_t) v->value);
	return;
}

// Codeword is allocated as part of specific variable_t
void _var4u_ref(){
	variable_t *v = (variable_t*) current_codeword->data;
	push(dataStack, (uint32_t) v->value);
	return;
}

// Codeword is allocated as part of specific variable_t
void _var8_ref(){
	variable_t *v = (variable_t*) current_codeword->data;
	push(dataStack, v->value);
	return;
}

codeword_t cw_var_undef_assign = {
	.name = "_var_undef_assign",
	.xt = _var_undef_assign,
	.text = "!",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_assign(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Undefined name %s assigned a value\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}


codeword_t cw_var_generic_assign = {
	.name = "_var_generic_assign",
	.xt = _var_generic_assign,
	.text = "!",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
// Assign top of stack to a variable
void _var_generic_assign(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for !\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	var->value = pop(dataStack);
	return;
}


codeword_t cw_var_undef_addrof = {
	.name = "_var_undef_addrof",
	.xt = _var_undef_addrof,
	.text = "'",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_addrof(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to get address of undefined name %s\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_addrof = {
	.name = "_var_generic_addrof",
	.xt = _var_generic_addrof,
	.text = "'",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_addrof(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	push(dataStack, (intptr_t)&(var->value));
	return;
}


codeword_t cw_var_undef_set0 = {
	.name = "_var_undef_set0",
	.xt = _var_undef_set0,
	.text = "!0",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_set0(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to set value of undefined name %s to 0\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_set0 = {
	.name = "_var_generic_set0",
	.xt = _var_generic_set0,
	.text = "!0",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_set0(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	var->value = 0;
	return;
}


codeword_t cw_var_undef_set1 = {
	.name = "_var_undef_set1",
	.xt = _var_undef_set1,
	.text = "!1",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_set1(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to set value of undefined name %s to 1\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_set1 = {
	.name = "_var_generic_set1",
	.xt = _var_generic_set1,
	.text = "!1",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_set1(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	var->value = 1;
	return;
}


codeword_t cw_var_undef_inc = {
	.name = "_var_undef_inc",
	.xt = _var_undef_inc,
	.text = "!1+",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_inc(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to increment value of undefined name %s\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_inc = {
	.name = "_var_generic_inc",
	.xt = _var_generic_inc,
	.text = "!1+",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_inc(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	var->value += 1;
	return;
}


codeword_t cw_var_undef_dec = {
	.name = "_var_undef_dec",
	.xt = _var_undef_dec,
	.text = "!1-",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_dec(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to decrement value of undefined name %s\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_dec = {
	.name = "_var_generic_dec",
	.xt = _var_generic_dec,
	.text = "!1-",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_dec(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	var->value -= 1;
	return;
}


codeword_t cw_var_undef_add = {
	.name = "_var_undef_add",
	.xt = _var_undef_add,
	.text = "!+",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_add(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to add to value of undefined name %s\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_add = {
	.name = "_var_generic_add",
	.xt = _var_generic_add,
	.text = "!+",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_add(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for !+\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	var->value += pop(dataStack);
	return;
}


codeword_t cw_var_undef_sub = {
	.name = "_var_undef_sub",
	.xt = _var_undef_sub,
	.text = "!-",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_sub(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to subtract from value of undefined name %s\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_sub = {
	.name = "_var_generic_sub",
	.xt = _var_generic_sub,
	.text = "!-",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_sub(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	FETCH_TOP_IND
	if(STACKEMPTY){
		fprintf(stderr,"ERR: Insufficient data operands for !-\n");
		abortExecution();
		cmdbuf->ip = -1; // word_next increments this!
		return;
	}
	var->value -= pop(dataStack);
	return;
}


codeword_t cw_var_undef_size = {
	.name = "_var_undef_size",
	.xt = _var_undef_size,
	.text = "SIZE",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell points to the undefined ref struct
void _var_undef_size(){
	cmdbuf->ip++; // Advance to data cell
	char * name =  (char *)(cmdbuf->array[cmdbuf->ip]);
	printf("ERR: Tried to get size of undefined name %s\n", name);
	abortExecution();
	cmdbuf->ip = -1; // word_next increments this!
	return;
}

codeword_t cw_var_generic_size = {
	.name = "_var_generic_size",
	.xt = _var_generic_size,
	.text = "SIZE",
	.size = 0,
	.data = 0,
	.next = NULL
};
// Next cell is a variable_t pointer
void _var_generic_size(){
	cmdbuf->ip++; // Advance to data cell
	variable_t * var =  (variable_t *)(cmdbuf->array[cmdbuf->ip]);
	push(dataStack, (intptr_t) (var->size));
	return;
}

