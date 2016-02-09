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

#ifndef DICT_H
#define DICT_H

#include "elem.h"
#include "stack.h"
#include "corewords.h"

/*	DICTIONARY HIERARCHY:

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
	void (*func)(stack *, elem *,dict *);
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
void wordRun(elem * sequence, stack * stack, dict * vocab);
// Attempts to define a new function
void defWord(elem * seq, dict * vocab);
// Defines built-in functions
void defCore(char * name, void (*func)(stack *, elem*, dict*), dict * vocab);

#endif
