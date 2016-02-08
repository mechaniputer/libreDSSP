#include <malloc.h>
#include <stdlib.h>
#include "corewords.h"
#include "elem.h"

elem * plus(elem * stack, elem * sequence){
	int sum;
	if((stack == NULL)||(stack->next == NULL)){
		fprintf(stderr,"ERROR: Insufficient operands for +\n");
		return stack;
	}

	elem * temp = stack->next; // For summing and because we free(stack)
	sum = temp->value + stack->value;
	free(stack); // Get rid of top element
	stack = temp; // We also need to get rid of this element
	temp = stack->next; // We will keep this element
	free(stack);
	stack = malloc(sizeof(elem)); // New element for top of stack
	stack->value = sum;
	stack->next = temp; // Need to make it be a stack!
	return stack;
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
