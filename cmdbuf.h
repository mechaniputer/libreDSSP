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

#ifndef CMDBUF_H
#define CMDBUF_H

#include <stdint.h>

#include "stack.h"

// Interpreter status flags (mostly for space-delimited input, but some may be used for other purposes)
#define STAT_INC_COMMENT   (1UL<<0)
#define STAT_INC_STRING    (1UL<<1)
#define STAT_INC_PRINT     (1UL<<2)
#define STAT_INC_COMPILE   (1UL<<3)  // For an incomplete : definition
#define STAT_INC_ESCAPE    (1UL<<4)  // For an escape char to apply to the next char
#define STAT_INC_DICT_OP   (1UL<<5)  // For GROW, USE, SHUT, ONLY
#define STAT_INC_VAR_DECL  (1UL<<6)  // For a VAR X statement that doesn't have X yet
#define STAT_INC_VAR_ASGN  (1UL<<7)  // For a ! X statement that doesn't have X yet
#define STAT_INC_VCTR 	   (1UL<<8)  // For vectors (eg 9 VCTR ROW, which declares a 10 element vector named ROW)
#define STAT_BR_NO_ELSE    (1UL<<9)  // For a BR that still needs an ELSE
#define STAT_BR_ELSE       (1UL<<10) // For a BR with an ELSE that still needs the last outcome.
#define STAT_INC_DO_LOOP   (1UL<<11) // For an incomplete DO-LOOP
#define STAT_INC_RP_LOOP   (1UL<<12) // For an incomplete RP-LOOP

typedef struct cmdbuffer_struct cmdbuffer;
typedef struct command_struct command;
typedef struct codeword codeword_t;

// This really belongs in dict.h but things were broken... TODO refactor
// TODO Add flag for :: definition (immune to CLEAR $v command)
struct codeword
{
	void (*xt)();  // Execution token (a function pointer)
	codeword_t * next;
	char * name;    // Word name
	char * text;    // For user words, the original definition. For printString, the string to print. Others can be NULL.
	intptr_t data; // Data payload: literal, pointer, or 0
	int size;      // Number of words in this word
	int user;      // 1 if user-defined, 0 if not.
};

struct cmdbuffer_struct
{
	codeword_t ** array;
	int capacity;
	int size;
	int status; // Used by parser to note incomplete phrases
	int ip; // Current instruction pointer
};

cmdbuffer * newCmdBuffer();
void cmdClear(cmdbuffer * cmdbuf);
void cmdAppend(cmdbuffer * cmdbuf, codeword_t * cw);
void cmdGrow(cmdbuffer * cmdbuf);
codeword_t * newLiteral(intptr_t value);
codeword_t * newVarDecl(char * name);
codeword_t * newVarAsgn(variable * dest);
codeword_t * newVarPush(variable * var);

#endif
