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

// Interpreter status flags
#define STAT_INC_COMMENT   (1UL<<0)
#define STAT_INC_STRING    (1UL<<1)
#define STAT_INC_PRINT     (1UL<<2)
#define STAT_INC_COMPILE   (1UL<<3)
#define STAT_INC_ESCAPE    (1UL<<4)


typedef struct cmdbuffer_struct cmdbuffer;
typedef struct command_struct command;
typedef struct codeword codeword_t;

// This really belongs in dict.h but things were broken... TODO refactor
// TODO Add flag for :: definition (immune to CLEAR $v command)
struct codeword
{
	void (*xt)();        // Execution token (a function pointer)
    intptr_t data;       // Data payload: literal, pointer, or 0
    const char *name;    // Word name
	char *text;
	codeword_t * next;
};

struct cmdbuffer_struct
{
	int capacity;
	int size;
	int status; // Used by parser to note incomplete phrases
	codeword_t ** array;

	int ip; // Current instruction pointer
};

cmdbuffer * newCmdBuffer();
void cmdClear(cmdbuffer * cmdbuf);
void cmdAppend(cmdbuffer * cmdbuf, codeword_t * cw);
void cmdGrow(cmdbuffer * cmdbuf);
codeword_t * newLiteral(intptr_t value);


#endif
