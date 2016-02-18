/*	This file is part of libreDSSP.

	Copyright 2016 Alan Beadle

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

typedef struct dict dict;

// Arithmetic
void plus(stack * stack, cmdstack * cmdstack, dict * vocab);
void multiply(stack * stack, cmdstack * cmdstack, dict * vocab);
void minus(stack * stack, cmdstack * cmdstack, dict * vocab);
void divide(stack * stack, cmdstack * cmdstack, dict * vocab);
void negate(stack * stack, cmdstack * cmdstack, dict * vocab);
void absval(stack * stack, cmdstack * cmdstack, dict * vocab);

// Display and interpreter
void bye(stack * stack, cmdstack * cmdstack, dict * vocab);
void showTop(stack * stack, cmdstack * cmdstack, dict * vocab);
void showStack(stack * stack, cmdstack * cmdstack, dict * vocab);

// Conditionals
void ifplus(stack * stack, cmdstack * cmdstack, dict * vocab);
void ifzero(stack * stack, cmdstack * cmdstack, dict * vocab);
void ifminus(stack * stack, cmdstack * cmdstack, dict * vocab);

// Looping and flow control
void doloop(stack * stack, cmdstack * cmdstack, dict * vocab);

// Stack manipulation
void exch2(stack * stack, cmdstack * cmdstack, dict * vocab);
void exch3(stack * stack, cmdstack * cmdstack, dict * vocab);
void exch4(stack * stack, cmdstack * cmdstack, dict * vocab);
void exchdepth(stack * stack, cmdstack * cmdstack, dict * vocab);
void copy(stack * stack, cmdstack * cmdstack, dict * vocab);
void copy2(stack * stack, cmdstack * cmdstack, dict * vocab);
void copy3(stack * stack, cmdstack * cmdstack, dict * vocab);
void copy4(stack * stack, cmdstack * cmdstack, dict * vocab);
void copydepth(stack * stack, cmdstack * cmdstack, dict * vocab);
void drop(stack * stack, cmdstack * cmdstack, dict * vocab);
void dropStack(stack * stack, cmdstack * cmdstack, dict * vocab);

// Misc
void defVar(stack * stack, cmdstack * cmdstack, dict * vocab);
void printNewline(stack * stack, cmdstack * cmdstack, dict * vocab);
void printSpace(stack * stack, cmdstack * cmdstack, dict * vocab);
void listDicts(stack * stack, cmdstack * cmdstack, dict * vocab);
void growSub(stack * stack, cmdstack * cmdstack, dict * vocab);
void shutSub(stack * stack, cmdstack * cmdstack, dict * vocab);
void openSub(stack * stack, cmdstack * cmdstack, dict * vocab);
void termInNum(stack * stack, cmdstack * cmdstack, dict * vocab);
void termOutNum(stack * stack, cmdstack * cmdstack, dict * vocab);
void stackDepth(stack * stack, cmdstack * cmdstack, dict * vocab);

#endif
