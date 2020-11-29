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

#ifndef DICT_H
#define DICT_H

#define CORE_NAME_LEN 8

#include "stack.h"
#include "cmdbuf.h"
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

// TODO add table of undefined words
// TODO add GC table

struct variable
{
	char name[16];
	int value;
	variable * next;
};

// TODO Add flag for :: definition (immune to CLEAR $v command)
struct word
{
	void *** code;
	word * next;
	char name[16]; // FIXME enforce or make dynamic
	char *text;
};

struct coreword
{
	void (*func)();
	char name[CORE_NAME_LEN];
	coreword * next;
};

struct subdict
{
	char * name;
	subdict * next;
	int open;
	word * wordlist;
};

struct dict
{
	coreword * core;
	subdict * sub;
	subdict * grow;
	variable * var;
};

// Looks for defined variables
variable * varSearch(char * name, dict * vocab);
// Looks for words to see if they are already defined
word * wordSearch(char * name, dict * vocab);
// Looks for core words to see if they are defined
coreword * coreSearch(char * name, dict * vocab);
// Attempts to define a new function
word * wordDefine(char * name, dict * vocab);
// Defines built-in functions
void defCore(char * name, void (*func)(), dict * vocab);
// Creates a new sub-dictionary
subdict * newDict(dict * vocab, char * name);

#endif
