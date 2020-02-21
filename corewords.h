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
#include "stack.h"
#include "cmdbuf.h"

typedef struct dict dict;

// Arithmetic
void plus(stack * stack, cmdbuffer * cmdbuf);
void multiply(stack * stack, cmdbuffer * cmdbuf);
void minus(stack * stack, cmdbuffer * cmdbuf);
void divide(stack * stack, cmdbuffer * cmdbuf);
void negate(stack * stack, cmdbuffer * cmdbuf);
void absval(stack * stack, cmdbuffer * cmdbuf);

void plus1(stack * stack, cmdbuffer * cmdbuf);
void plus2(stack * stack, cmdbuffer * cmdbuf);
void plus3(stack * stack, cmdbuffer * cmdbuf);
void plus4(stack * stack, cmdbuffer * cmdbuf);

void minus1(stack * stack, cmdbuffer * cmdbuf);
void minus2(stack * stack, cmdbuffer * cmdbuf);
void minus3(stack * stack, cmdbuffer * cmdbuf);
void minus4(stack * stack, cmdbuffer * cmdbuf);

// Display and interpreter
void bye(stack * stack, cmdbuffer * cmdbuf);
void showTop(stack * stack, cmdbuffer * cmdbuf);
void showStack(stack * stack, cmdbuffer * cmdbuf);
void base10(stack * stack, cmdbuffer * cmdbuf);

// Conditionals
void ifplus(stack * stack, cmdbuffer * cmdbuf);
void ifzero(stack * stack, cmdbuffer * cmdbuf);
void ifminus(stack * stack, cmdbuffer * cmdbuf);
void branchminus(stack * stack, cmdbuffer * cmdbuf);
void branchzero(stack * stack, cmdbuffer * cmdbuf);
void branchplus(stack * stack, cmdbuffer * cmdbuf);
void branchsign(stack * stack, cmdbuffer * cmdbuf);
void branch(stack * stack, cmdbuffer * cmdbuf);
void equality(stack * stack, cmdbuffer * cmdbuf);
void greaterthan(stack * stack, cmdbuffer * cmdbuf);
void lessthan(stack * stack, cmdbuffer * cmdbuf);

// Looping
void doloop(stack * stack, cmdbuffer * cmdbuf);

// Stack manipulation
void exch2(stack * stack, cmdbuffer * cmdbuf);
void exch3(stack * stack, cmdbuffer * cmdbuf);
void exch4(stack * stack, cmdbuffer * cmdbuf);
void exchdepth(stack * stack, cmdbuffer * cmdbuf);
void copy(stack * stack, cmdbuffer * cmdbuf);
void copy2(stack * stack, cmdbuffer * cmdbuf);
void copy3(stack * stack, cmdbuffer * cmdbuf);
void copy4(stack * stack, cmdbuffer * cmdbuf);
void copydepth(stack * stack, cmdbuffer * cmdbuf);
void drop(stack * stack, cmdbuffer * cmdbuf);
void dropStack(stack * stack, cmdbuffer * cmdbuf);

// Misc
void defVar(stack * stack, cmdbuffer * cmdbuf);
void printNewline(stack * stack, cmdbuffer * cmdbuf);
void printSpace(stack * stack, cmdbuffer * cmdbuf);
void listDicts(stack * stack, cmdbuffer * cmdbuf);
void growSub(stack * stack, cmdbuffer * cmdbuf);
void shutSub(stack * stack, cmdbuffer * cmdbuf);
void openSub(stack * stack, cmdbuffer * cmdbuf);
void termInNum(stack * stack, cmdbuffer * cmdbuf);
void termOutNum(stack * stack, cmdbuffer * cmdbuf);
void stackDepth(stack * stack, cmdbuffer * cmdbuf);

#endif
