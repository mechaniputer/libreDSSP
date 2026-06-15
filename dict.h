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
	User words and variables are stored in subdicts, which are linked together in a linked list in the dict struct.
	One subdict at a time can be selected for new word definitions and variable declarations
	Individual subdicts can be open or closed to lookups.


                      variable    variable    variable
                        /           /           /
                    variable    variable    variable
                      /           /           /
                     /           /           /
         dict --- subdict --- subdict --- subdict
        /            \           \           \
       /              \           \           \
  codeword_t       codeword_t  codeword_t  codeword_t
      |                \           \           \
  codeword_t        codeword_t  codeword_t  codeword_t
      |                 \           \           \
  codeword_t         codeword_t  codeword_t  codeword_t
      |
  codeword_t

*/

typedef struct undefined_ref undefined_ref_t;

typedef struct codeword codeword_t;
typedef struct variable_t variable_t;
typedef struct subdict subdict;
typedef struct dict dict;

struct undefined_ref {
	char *name;                      // name of undefined word/var
	codeword_t *** references;        // array of pointers to codewords that reference this
	codeword_t * ref_placeholder;    // The word that all of the plain refs point to
	codeword_t * assign_placeholder; // The word that assign refs point to
	undefined_ref_t *next;          // next entry in linked list
	int ref_count;                   // number of references
	int ref_capacity;                // allocated space for references array
};

struct variable_t
{
	char * name;
	variable_t * next;
	codeword_t cw[2]; // 0:pushVar, 1:assignVar
	int value;
};

struct subdict
{
	char * name;
	subdict * next;
	codeword_t * wordlist;
	variable_t * varlist;
	int open;
};

struct dict
{
	codeword_t * core;
	subdict * sub;
	subdict * grow;
	undefined_ref_t * undefined;
};

// Check if a name is already used for a word/var
int collisionSearch(char * name, dict * vocab);
// Looks for defined variables
variable_t * varSearch(char * name, dict * vocab);
// Looks for words to see if they are already defined
codeword_t * wordSearch(char * name, dict * vocab);
// Looks for core words to see if they are defined
codeword_t * coreSearch(char * name, dict * vocab);
// Attempts to define a new function
codeword_t * wordDefine(char * name, dict * vocab);
// Defines built-in functions
void defCore(char * name, void (*func)(), dict * vocab);
// Creates a new sub-dictionary
subdict * newDict(char * name, dict * vocab);
// Finds a sub-dictionary by name, returns NULL if not found
subdict * findDict(char * name, dict * vocab);

// Utility functions for undefined word table
undefined_ref_t* undefSearch(char *name, dict *vocab);
undefined_ref_t* create_undefined_ref(char *name, dict *vocab);
void add_reference(undefined_ref_t *undef, codeword_t **ref);
void resolve_undefined_ref(char *name, codeword_t *def, int isVar, variable_t * var, dict *vocab);

#endif
