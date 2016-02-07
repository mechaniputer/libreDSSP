#include <stdio.h>
#include "dict.h"
#include "elem.h"
#include "corewords.h"

elem * wordFind(char * elemName, elem * stack, dict * vocab){
	coreword * tempCore;
	// Search core dict first
	tempCore = vocab->core;
	if(!strcmp(tempCore->name, elemName)){
		stack = tempCore->func(stack);
	}
	return stack;
}
