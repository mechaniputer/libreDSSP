#include <stdio.h>
#include "dict.h"
#include "elem.h"
#include "corewords.h"

elem * wordFind(char * elemName, elem * stack, dict * vocab){
	coreword * tempCore;
	// Search core dict first
	tempCore = vocab->core;
	do{
		if(!strcmp(tempCore->name, elemName)){
			stack = tempCore->func(stack);
		}
		tempCore = tempCore->next;
	}while(tempCore != NULL);
	return stack;
}
