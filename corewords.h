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

#ifndef COREWORDS_H
#define COREWORDS_H

#include <stdlib.h>
#include "elem.h"
#include "stack.h"
#include "cmdbuf.h"

typedef struct dict dict;

// Arithmetic
void plus(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void multiply(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void minus(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void divide(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void negate(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void absval(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

void plus1(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void plus2(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void plus3(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void plus4(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

void minus1(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void minus2(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void minus3(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void minus4(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

// Display and interpreter
void bye(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void showTop(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void showStack(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void base10(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

// Conditionals
void ifplus(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void ifzero(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void ifminus(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void branchminus(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void branchzero(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void branchplus(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void branchsign(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void branch(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void equality(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void greaterthan(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void lessthan(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

// Looping
void doloop(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

// Stack manipulation
void exch2(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void exch3(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void exch4(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void exchdepth(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void copy(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void copy2(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void copy3(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void copy4(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void copydepth(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void drop(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void dropStack(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

// Misc
void defVar(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void printNewline(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void printSpace(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void listDicts(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void growSub(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void shutSub(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void openSub(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void termInNum(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void termOutNum(stack * stack, cmdbuffer * cmdbuf, dict * vocab);
void stackDepth(stack * stack, cmdbuffer * cmdbuf, dict * vocab);

#endif
