#include <stdio.h>
#include <malloc.h>

#include "dict.h"
#include "elem.h"
#include "corewords.h"
#include "util.h"

int main(){
	elem * seqHead;
	elem * stack = NULL;

	dict * vocab = malloc(sizeof(dict)); // Contains all recognized words
	vocab->sub = malloc(sizeof(subdict)); // For user defined words, can add dicts later

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
	// Misc
	defCore("D", drop, vocab);
	defCore("C", copy, vocab);

	// Sub-Dictionaries
	vocab->sub->next = NULL;
	vocab->sub->wordlist = NULL;

	while(1){
		// Show prompt, get line of input
		seqHead = parseInput(prompt());
		stack = run(stack, seqHead, vocab);
	}
	return 0;
}
