#include <malloc.h>
#include "corewords.h"
#include "elem.h"

elem * plus(elem * stack){
	int sum;

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
