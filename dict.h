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

#ifndef DICT_H
#define DICT_H

#define CORE_NAME_LEN 8

#include "stack.h"
#include "cmdbuf.h"
#include "corewords.h"

/*	DICTIONARY HIERARCHY:
	Core words are stored in a linked list in the dict struct. They are always present.
	User words are stored in subdicts, which are linked together in a linked list in the dict struct.
	Individual subdicts can be open or closed.
	
         dict --- variable --- variable --- variable
        /    \
       /       subdict --- subdict --- subdict
  codeword_t      |           |           |
      |       codeword_t  codeword_t  codeword_t
  codeword_t      |           |           |
      |       codeword_t  codeword_t  codeword_t
  codeword_t      |           |           |
      |       codeword_t  codeword_t  codeword_t
  codeword_t

*/

typedef struct codeword codeword_t;
typedef struct variable variable;
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

struct subdict
{
	char * name;
	subdict * next;
	int open;
	codeword_t * wordlist;
};

struct dict
{
	codeword_t * core;
	subdict * sub;
	subdict * grow;
	variable * var;
};

// Looks for defined variables
variable * varSearch(char * name, dict * vocab);
// Looks for words to see if they are already defined
codeword_t * wordSearch(char * name, dict * vocab);
// Looks for core words to see if they are defined
codeword_t * coreSearch(char * name, dict * vocab);
// Attempts to define a new function
codeword_t * wordDefine(char * name, dict * vocab);
// Defines built-in functions
void defCore(char * name, void (*func)(), dict * vocab);
// Creates a new sub-dictionary
subdict * newDict(dict * vocab, char * name);

#endif
