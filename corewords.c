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
#include "stack.h"

void plus(stack * stack, elem * sequence, dict * vocab){
	int temp;
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for +\n");
		return;
	}
	temp = pop(stack);
	stack->array[stack->top] += temp;
	return;
}

void multiply(stack * stack, elem * sequence, dict * vocab){
	int temp1;
	int temp2;
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for *\n");
		return;
	}
	temp1 = pop(stack);
	temp2 = top(stack);
	stack->array[stack->top] = temp1 * temp2;
	return;
}

void minus(stack * stack, elem * sequence, dict * vocab){
	int temp;
	// -1 indicates empty stack
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for -\n");
		return;
	}
	temp = pop(stack);
	stack->array[stack->top] -= temp;
	return;
}

void divide(stack * stack, elem * sequence, dict * vocab){
	int temp1;
	int temp2;
	if(stack->top <= 0){
		fprintf(stderr,"ERROR: Insufficient operands for /\n");
		return;
	}
	temp1 = pop(stack);
	temp2 = top(stack);
	stack->array[stack->top] = temp2 / temp1;
	return;
}

void negate(stack * stack, elem * sequence, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for NEG\n");
		return;
	}
	stack->array[stack->top] = -1 *stack->array[stack->top];
	return;
}

void absval(stack * stack, elem * sequence, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for ABS\n");
		return;
	} else if(top(stack) < 0) {
		stack->array[stack->top] = -1 * (stack->array[stack->top]);
	}
	return;
}

void bye(stack * stack, elem * sequence, dict * vocab){
	printf("Exiting libreDSSP\n");
	exit(0);
}

// TODO This will need to be modified to support multiple output modes
void showTop(stack * stack, elem * sequence, dict * vocab){
	if(stack->top > -1) printf("%d",top(stack));
	printf("\n");
	return;
}

void showStack(stack * stack, elem * sequence, dict * vocab){
	int i;
	if(stack->top < 0){
		printf("\n");
		return;
	}

	for(i=0; i<=(stack->top); i++){
		printf("%d ",stack->array[i]);
	}
	printf("\n");
	return;
}

void ifplus(stack * stack, elem * sequence, dict * vocab){
	if((sequence->next == NULL ) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		return;
	}

	if(pop(stack) <= 0){
		sequence->next = sequence->next->next;
	}
	return;
}

void ifzero(stack * stack, elem * sequence, dict * vocab){
	if((sequence->next == NULL ) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF0\n");
		return;
	}

	if(pop(stack) != 0){
		sequence->next = sequence->next->next;
	}
	return;
}

void ifminus(stack * stack, elem * sequence, dict * vocab){
	if((sequence->next == NULL ) || (stack->top < 0)){
		fprintf(stderr,"ERROR: Insufficient operands for IF+\n");
		return;
	}

	if(pop(stack) >= 0){
		sequence->next = sequence->next->next;
	}
	return;
}


void doloop(stack * stack, elem * sequence, dict * vocab){
	int i;
	elem * repeat = sequence->next;
	if((sequence->next == NULL) || (stack->top < 0)) {
		fprintf(stderr,"ERROR: Insufficient operands for DO\n");
		return;
	}

	int reps = pop(stack);

	// Isolate the repeated word
	sequence->next = sequence->next->next;
	repeat->next = NULL;
	for(i = 0; i < reps; i++){
		run(stack, repeat, vocab);
	}
	return;
}

void drop(stack * stack, elem * sequence, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for D\n");
		return;
	}
	pop(stack);
	return;
}

void copy(stack * stack, elem * sequence, dict * vocab){
	if(stack->top < 0){
		fprintf(stderr,"ERROR: Insufficient operands for C\n");
		return;
	}
	push(stack,top(stack));
	return;
}

