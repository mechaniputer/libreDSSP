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

        dict --- variable --- variable --- variable
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

typedef struct variable variable;
typedef struct word word;
typedef struct coreword coreword;
typedef struct subdict subdict;
typedef struct dict dict;

struct variable
{
	char name[8];
	int value;
	variable * next;
};

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
	void (*func)(stack *, cmdstack *,dict *);
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
	variable * var;
};

// Looks for defined variables
variable * varSearch(char * name, dict * vocab);
// Looks for words to see if they are already defined
word * wordSearch(char * name, dict * vocab);
// Looks for core words to see if they are defined
int coreSearch(char * name, dict * vocab);
// Attempts to define a new function
void defWord(cmdstack * cmdstack, dict * vocab);
// Defines built-in functions
void defCore(char * name, void (*func)(stack *, cmdstack *, dict *), dict * vocab);

#endif
