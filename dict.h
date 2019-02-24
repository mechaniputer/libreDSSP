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

#include "elem.h"
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

struct variable
{
	char name[16];
	int value;
	variable * next;
};

// TODO: array of commands
struct word
{
	char name[16];
	int length;
	int capacity;
	command * array; // TODO: Make dynamic
	word * next;
};

struct coreword
{
	char name[8];
	coreword * next;
	void (*func)(stack *, cmdbuffer *, dict *);
};

struct subdict
{
	char * name;
	int open;
	word * wordlist;
	subdict * next;
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
void defWord(cmdbuffer * cmdbuf, dict * vocab);
void growWord(word * word, char * com, dict * vocab);
// Defines built-in functions
void defCore(char * name, void (*func)(stack *, cmdbuffer *, dict *), dict * vocab);
subdict * newDict(dict * vocab, char * name);

#endif
