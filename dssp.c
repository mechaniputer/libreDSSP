/*	This file is part of libreDSSP.

	Copyright 2019 Alan Beadle

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

int main(int argc, char *argv[]){
	stack * workStack = newStack();
	cmdstack * cmdstack = newCmdStack();

	dict * vocab = malloc(sizeof(dict)); // Contains all recognized words
	vocab->core = NULL;
	vocab->grow = NULL;
	vocab->var = NULL;

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
	defCore("DEEP", stackDepth, vocab);

	// Sub-Dictionaries
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add more dicts later
	vocab->sub->name = "$DEFAULT";
	vocab->sub->open = 1;
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;
	vocab->grow = vocab->sub; // We will grow this dictionary by default

	// Version
	printf("\nlibreDSSP, version 0.0.1\n");

	// Copyright notice
	printf("Copyright (C) 2019  Alan Beadle\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");

	if(argc >= 2){
		printf("Attempting to open %s... ",argv[1]);
		FILE *file = fopen(argv[1], "r");
		char * bufptr = NULL;
		size_t bufsize = 0;
		size_t characters;

		if (file == 0){
			printf("Failed!\n");
		}else{
			printf("Success!\n");
			while(EOF != (characters = getline(&bufptr, &bufsize, file))){
				bufptr[characters-1] = '\0';
				stackInput(bufptr, cmdstack);
				run(workStack, cmdstack, vocab);
				free(bufptr);
				bufsize = 0;
			}
			fclose(file);
		}
	}

	while(1){
		// Show prompt, get line of input
		stackInput(prompt(cmdstack->unfinished_comment || cmdstack->unfinished_func), cmdstack);
		run(workStack, cmdstack, vocab);
	}
	return 0;
}
