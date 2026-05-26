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

typedef struct undefined_word undefined_word_t;

typedef struct codeword codeword_t;
typedef struct variable variable;
typedef struct subdict subdict;
typedef struct dict dict;

// TODO add GC table

struct undefined_word {
	char *name;                    // name of undefined word
	codeword_t **references;       // array of pointers to codewords that reference this
	int ref_count;                 // number of references
	int ref_capacity;              // allocated space for references array
	codeword_t * placeholder;      // The word that all of the refs point to
	undefined_word_t *next;        // next entry in linked list
};

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
	undefined_word_t *undefined;
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
// Finds a sub-dictionary by name, returns NULL if not found
subdict * findDict(dict * vocab, char * name);

// Utility functions for undefined word table
undefined_word_t* find_undefined_word(dict *d, char *name);
undefined_word_t* create_undefined_word(dict *d, char *name);
void add_reference(undefined_word_t *undef, codeword_t *ref);
void resolve_undefined_word(dict *vocab, char *name, codeword_t *def);

#endif
