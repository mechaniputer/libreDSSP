/*	This file is part of libreDSSP.

	Copyright 2016 Alan Beadle

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
#include "corewords.h"
#include "dict.h"
#include "elem.h"

elem * plus(elem * stack, elem * sequence, dict * vocab){
	int sum;
	if((stack == NULL) || (stack->next == NULL)){
		fprintf(stderr,"ERROR: Insufficient operands for +\n");
		return stack;
	}
	elem * temp = stack->next; // For summing and because we free(stack)
	sum = temp->value + stack->value;
	free(stack); // Get rid of top element
	stack = temp; // We also need to revalue this element
	stack->value = sum;
	return stack;
}

elem * multiply(elem * stack, elem * sequence, dict * vocab){
	int product;
	if((stack == NULL) || (stack->next == NULL)){
		fprintf(stderr,"ERROR: Insufficient operands for +\n");
		return stack;
	}
	elem * temp = stack->next; // For multiplying and because we free(stack)
	product = temp->value * stack->value;
	free(stack); // Get rid of top element
	stack = temp; // We also need to revalue this element
	stack->value = product;
	return stack;
}

elem * minus(elem * stack, elem * sequence, dict * vocab){
	int diff;
	if((stack == NULL) || (stack->next == NULL)){
		fprintf(stderr,"ERROR: Insufficient operands for -\n");
		return stack;
	}
	elem * temp = stack->next; // For diff and because we free(stack)
	diff = (temp->value) - (stack->value);
	free(stack); // Get rid of top element
	stack = temp; // We also need to revalue this element
	stack->value = diff;
	return stack;
}

elem * divide(elem * stack, elem * sequence, dict * vocab){
	int quotient;
	if((stack == NULL) || (stack->next == NULL)){
		fprintf(stderr,"ERROR: Insufficient operands for /\n");
		return stack;
	}
	elem * temp = stack->next; // For dividing and because we free(stack)
	quotient = temp->value / stack->value;
	free(stack); // Get rid of top element
	stack = temp; // We also need to revalue this element
	stack->value = quotient;
	return stack;
}

elem * negate(elem * stack, elem * sequence, dict * vocab){
	if(stack == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for NEG\n");
		return stack;
	}
	stack->value = -1 * (stack->value);
}

elem * absval(elem * stack, elem * sequence, dict * vocab){
	if(stack == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for ABS\n");
		return stack;
	} else if(stack->value < 0) {
		stack->value = -1 * (stack->value);
	} else return stack;
}

elem * bye(elem * stack, elem * sequence, dict * vocab){
	printf("Exiting libreDSSP\n");
	exit(0);
}

// TODO This will need to be modified to support multiple output modes
elem * showTop(elem * stack, elem * sequence, dict * vocab){
	if(stack != NULL){
		printf("%d\n",stack->value);
	}
	return stack;
}

elem * showStack(elem * stack, elem * sequence, dict * vocab){
	if(stack == NULL) return stack;

	elem * temp = stack;
	elem * bottom = NULL;

	while(bottom != stack->next){ // While we haven't printed the top
		temp = stack; // Start at the top
		// Find lowest not printed
		do{
			temp = temp->next;
		}while(temp->next != bottom);

		printf("%d ",temp->value);
		bottom = temp;
	}
	printf("%d ",stack->value); // Now print the top

	printf("\n");
	return stack;
}

elem * ifplus(elem * stack, elem * sequence, dict * vocab){
	if(sequence->next == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		return stack;
	}
	if((stack == NULL) || (stack->value <= 0)) {
		sequence->next = sequence->next->next;
	}
	return stack;
}

elem * ifzero(elem * stack, elem * sequence, dict * vocab){
	if(sequence->next == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for IF0\n");
		return stack;
	}
	if((stack == NULL) || (stack->value != 0)) {
		sequence->next = sequence->next->next;
	}
	return stack;
}

elem * ifminus(elem * stack, elem * sequence, dict * vocab){
	if(sequence->next == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for IF-\n");
		return stack;
	}
	if((stack == NULL) || (stack->value >= 0)) {
		sequence->next = sequence->next->next;
	}
	return stack;
}

elem * doloop(elem * stack, elem * sequence, dict * vocab){
	int i;
	int reps = stack->value;

	elem * repeat = sequence->next;
	if((sequence->next == NULL) || (stack == NULL)) {
		fprintf(stderr,"ERROR: Insufficient operands for DO\n");
		return stack;
	}

	// Remove top stack value
	elem * temp = stack->next;
	free(stack);
	stack = temp;

	// Isolate the repeated word
	sequence->next = sequence->next->next;
	repeat->next = NULL;
	for(i = 0; i < reps; i++){
		stack = run(stack, repeat, vocab);
	}
	return stack;
}

elem * drop(elem * stack, elem * sequence, dict * vocab){
	if(stack == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for D\n");
		return stack;
	}
	elem * temp = stack->next;
	free(stack);
	stack = temp;
	return stack;
}

elem * copy(elem * stack, elem * sequence, dict * vocab){
	if(stack == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for C\n");
		return stack;
	}
	elem * temp = stack;
	stack = malloc(sizeof(elem));
	stack->value = temp->value;
	stack->next = temp;
	return stack;
}

