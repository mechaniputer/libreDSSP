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


// Codeword structure for indirect threading
// Each executable unit (core word, user word, literal) is represented as a codeword
typedef struct {
    void (*xt)();        // Execution token (a function pointer)
    intptr_t data;       // Data payload: literal, pointer, or 0
    const char *name;    // Word name
} codeword_t;

typedef struct cmdbuffer_struct cmdbuffer;
typedef struct command_struct command;


struct cmdbuffer_struct
{
	int capacity;
	int size;
	int status; // Used by parser to note incomplete phrases
	void *** array;

	int ip; // Current instruction pointer
};

cmdbuffer * newCmdBuffer();
void cmdClear(cmdbuffer * cmdbuf);
void cmdAppend(cmdbuffer * cmdbuf, void * cmd);
void cmdGrow(cmdbuffer * cmdbuf);


#endif
