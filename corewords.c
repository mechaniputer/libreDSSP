#include <malloc.h>
#include <stdlib.h>
#include "corewords.h"
#include "elem.h"

elem * plus(elem * stack, elem * sequence){
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

elem * multiply(elem * stack, elem * sequence){
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

elem * minus(elem * stack, elem * sequence){
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

elem * divide(elem * stack, elem * sequence){
	return stack;
}

elem * negate(elem * stack, elem * sequence){
	if(stack == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for NEG\n");
		return stack;
	}
	stack->value = -1 * (stack->value);
}

elem * absval(elem * stack, elem * sequence){
	if(stack == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for ABS\n");
		return stack;
	} else if(stack->value < 0) {
		stack->value = -1 * (stack->value);
	} else return stack;
}

elem * bye(elem * stack, elem * sequence){
	printf("Exiting DSSP\n");
	exit(0);
}

// TODO This will need to be modified to support multiple output modes
elem * showTop(elem * stack, elem * sequence){
	if(stack != NULL){
		printf("%d\n",stack->value);
	}
	return stack;
}

elem * showStack(elem * stack, elem * sequence){
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

elem * ifplus(elem * stack, elem * sequence){
	// This will segfault if conditional is last element in sequence
	if((stack == NULL) || (stack->value <= 0)) {
		sequence->next = sequence->next->next;
	}
	return stack;
}

elem * ifzero(elem * stack, elem * sequence){
	// This will segfault if conditional is last element in sequence
	if((stack == NULL) || (stack->value != 0)) {
		sequence->next = sequence->next->next;
	}
	return stack;
}

elem * ifminus(elem * stack, elem * sequence){
	// This will segfault if conditional is last element in sequence
	if((stack == NULL) || (stack->value >= 0)) {
		sequence->next = sequence->next->next;
	}
	return stack;
}

elem * drop(elem * stack, elem * sequence){
	if(stack == NULL){
		fprintf(stderr,"ERROR: Insufficient operands for D\n");
		return stack;
	}
	elem * temp = stack->next;
	free(stack);
	stack = temp;
	return stack;
}

elem * copy(elem * stack, elem * sequence){
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

