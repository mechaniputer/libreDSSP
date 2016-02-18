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

#include <stdio.h>
#include <malloc.h>

#include "dict.h"
#include "elem.h"
#include "stack.h"
#include "corewords.h"
#include "util.h"

int main(){
	stack * stack = newStack();
	cmdstack * cmdstack = newCmdStack();

	dict * vocab = malloc(sizeof(dict)); // Contains all recognized words
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add more dicts later
	vocab->sub->name = "$DEFAULT";
	vocab->sub->open = 1;
	vocab->sub->next = NULL;

	// Built-ins
	vocab->core = malloc(sizeof(coreword)); // For built-in words

	// Arithmetic
	defCore("+", plus, vocab);
	defCore("*", multiply, vocab);
	defCore("-", minus, vocab);
	defCore("/", divide, vocab);
	defCore("NEG", negate, vocab);
	defCore("ABS", absval, vocab);
	// Display and interpreter
	defCore("BYE", bye, vocab);
	defCore(".", showTop, vocab);
	defCore("..", showStack, vocab);
	// Conditionals
	defCore("IF+", ifplus, vocab);
	defCore("IF0", ifzero, vocab);
	defCore("IF-", ifminus, vocab);
	// Looping and flow control
	defCore("DO", doloop, vocab);
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
	defCore("!", defVar, vocab);
	defCore("CR", printNewline, vocab);
	defCore("SP", printSpace, vocab);
	defCore("?$", listDicts, vocab);
	defCore("GROW", growSub, vocab);
	defCore("SHUT", shutSub, vocab);
	defCore("USE", openSub, vocab);
	defCore("TIN", termInNum, vocab);
	defCore("TON", termOutNum, vocab);
	defCore("DEPTH", stackDepth, vocab);

	// Sub-Dictionaries
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;

	// Version
	printf("\nlibreDSSP, version 0.0.0\n");

	// Copyright notice
	printf("Copyright (C) 2016  Alan Beadle\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");

	while(1){
		// Show prompt, get line of input
		stackInput(prompt(),cmdstack);
		run(stack, cmdstack, vocab);
	}
	return 0;
}
