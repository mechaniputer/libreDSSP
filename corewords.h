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

typedef struct dict dict;

// Arithmetic
elem * plus(elem * stack, elem * sequence, dict * vocab);
elem * multiply(elem * stack, elem * sequence, dict * vocab);
elem * minus(elem * stack, elem * sequence, dict * vocab);
elem * divide(elem * stack, elem * sequence, dict * vocab);
elem * negate(elem * stack, elem * sequence, dict * vocab);
elem * absval(elem * stack, elem * sequence, dict * vocab);

// Display and interpreter
elem * bye(elem * stack, elem * sequence, dict * vocab);
elem * showTop(elem * stack, elem * sequence, dict * vocab);
elem * showStack(elem * stack, elem * sequence, dict * vocab);

// Conditionals
elem * ifplus(elem * stack, elem * sequence, dict * vocab);
elem * ifzero(elem * stack, elem * sequence, dict * vocab);
elem * ifminus(elem * stack, elem * sequence, dict * vocab);

// Looping and flow control
elem * doloop(elem * stack, elem * sequence, dict * vocab);

// Misc
elem * drop(elem * stack, elem * sequence, dict * vocab);
elem * copy(elem * stack, elem * sequence, dict * vocab);

#endif
