#ifndef DICT_H
#define DICT_H

#include "elem.h"
#include "corewords.h"

/*

        dict
       /    \
      /     subdict --- subdict --- subdict
  coreword     |           |           |
     |        word        word        word
  coreword     |           |           |
     |        word        word        word
  coreword     |           |           |
     |        word        word        word
  coreword

*/

typedef struct word word;
typedef struct coreword coreword;
typedef struct subdict subdict;
typedef struct dict dict;
struct word
{
	char name[8];
	char * definition;
	word * next;
};

struct coreword
{
	char name[8];
	coreword * next;
	elem * (*func)(elem *);
};

struct subdict
{
	word * wordlist;
	subdict * next;
};

struct dict
{
	coreword * core;
	subdict * sub;
};

// Searches dictionaries, runs a word if possible
elem * wordFind(char * elemName, elem * stack, dict * vocab);
// Attempts to define a new function
elem * funcDec(elem * seq, dict * vocab);

#endif
