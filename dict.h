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
	char definition[80];
	word * next;
};

struct coreword
{
	char name[8];
	coreword * next;
	elem * (*func)(elem *, elem *);
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

// Looks for words to see if they are already defined
word * wordSearch(char * name, dict * vocab);
// Looks for core words to see if they are defined
int coreSearch(char * name, dict * vocab);
// Searches dictionaries, runs a word if possible
elem * wordRun(elem * sequence, elem * stack, dict * vocab);
// Attempts to define a new function
elem * defWord(elem * seq, dict * vocab);
// Defines built-in functions
void defCore(char * name, elem * (*func)(elem *, elem*), dict * vocab);

#endif
