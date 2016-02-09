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
void plus(stack * stack, elem * sequence, dict * vocab);
void multiply(stack * stack, elem * sequence, dict * vocab);
void minus(stack * stack, elem * sequence, dict * vocab);
void divide(stack * stack, elem * sequence, dict * vocab);
void negate(stack * stack, elem * sequence, dict * vocab);
void absval(stack * stack, elem * sequence, dict * vocab);

// Display and interpreter
void bye(stack * stack, elem * sequence, dict * vocab);
void showTop(stack * stack, elem * sequence, dict * vocab);
void showStack(stack * stack, elem * sequence, dict * vocab);

// Conditionals
void ifplus(stack * stack, elem * sequence, dict * vocab);
void ifzero(stack * stack, elem * sequence, dict * vocab);
void ifminus(stack * stack, elem * sequence, dict * vocab);

// Looping and flow control
void doloop(stack * stack, elem * sequence, dict * vocab);

// Misc
void drop(stack * stack, elem * sequence, dict * vocab);
void copy(stack * stack, elem * sequence, dict * vocab);

#endif
