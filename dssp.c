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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tokparse.h"
#include "dict.h"
#include "stack.h"
#include "cmdbuf.h"
#include "corewords.h"
#include "util.h"

cmdbuffer *cmdbuf;
stack *dataStack;
stack *returnStack;
stack * loopStack;
stack * tokenizerStack;
stack * parserStack;
dict * vocab; // Contains all recognized words, including core and user defined
int abort_requested = 0;

int main(int argc, char *argv[]){
	cmdbuf = newCmdBuffer();
	dataStack = newStack();
	returnStack = newStack();
	loopStack = newStack();
	tokenizerStack = newStack();
	parserStack = newStack();

	vocab = malloc(sizeof(dict));
	vocab->core = NULL;
	vocab->grow = NULL;
	vocab->undefined = NULL;

	// Most common literals
	defCore("0", push_zero, vocab);
	defCore("1", push_one, vocab);
	defCore("2", push_two, vocab);
	defCore("4", push_four, vocab);
	defCore("8", push_eight, vocab);

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
	defCore("DO", do_begin, vocab);
	defCore("DO_LOOP", do_loop, vocab); // Not to be used directly
	defCore("RP", rp_begin, vocab);
	defCore("RP_LOOP", rp_loop, vocab); // Not to be used directly
	defCore("EX", loop_exit, vocab);

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
	// TODO for special functions we should just use references instead of the dictionary.
	defCore("PUSHLIT", pushLiteral, vocab);
	//defCore("DOCOLON", word_enter, vocab); // Not to be used directly
	defCore(";S", word_exit, vocab); // Not to be used directly
	defCore("SKP1", skip1, vocab); // Not to be used directly
	defCore("SKP2", skip2, vocab); // Not to be used directly
	defCore("NOP", noop, vocab);
	defCore("VAR", declareVar, vocab);
	defCore("!", assignVar, vocab);
	defCore("PUSHVAR", pushVar, vocab); // Not to be used directly
	defCore("CR", printNewline, vocab);
	defCore("SP", printSpace, vocab);
	defCore("?$", listDicts, vocab);
	defCore("GROW", growSub, vocab);
	defCore("SHUT", shutSub, vocab);
	defCore("USE", openSub, vocab);
	defCore("TIN", termInNum, vocab);
	defCore("TON", termOutNum, vocab);
	defCore("TOS", termOutString, vocab);
	defCore("DEEP", stackDepth, vocab);
	defCore("UNDEF",inventoryUndefined, vocab);
	defCore("WORDS", inventoryWords, vocab); // Borrowed from FORTH. Currently unsure if DSSP had an equivalent.
	// TODO should add words that check word size of the machine (64 or 32 bits) to enable portable DSSP code.

	// Sub-Dictionaries
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add more dicts later
	vocab->sub->name = "$DEFAULT";
	vocab->sub->open = 1;
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;
	vocab->sub->varlist = NULL;
	vocab->grow = vocab->sub; // We will grow this dictionary by default

	// Version
	printf("\nlibreDSSP, version 0.6.0\n");

	// Copyright notice
	printf("Copyright (C) 2026  Alan Beadle\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
	int status = 0;

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
				status = process_line(bufptr);
				if(status == 0) word_next();
				free(bufptr);
				bufptr = NULL;
				bufsize = 0;
				cmdbuf->size = 0;
				cmdbuf->array[0] = NULL;
			}
			fclose(file);
		}
	}

	while(1){
		assert(-1 == returnStack->top); // Ensure we have an empty return stack
		assert(-1 == loopStack->top); // Ensure we have an empty loop stack
		abort_requested = 0;
		// Show prompt, get line of input
		char * line = prompt(status);
		status = process_line(line);
		free(line);
		//print_codewords(cmdbuf->array);
		if(status == 0) word_next();
		cmdbuf->size = 0;
		cmdbuf->array[0] = NULL;
	}
	return 0;
}
