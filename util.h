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

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "dict.h"
#include "elem.h"

int isnum(char * foo);
elem * run(elem * stack, elem * seqHead, dict * vocab);
elem * parseInput(char * line);
char * prompt();

#endif
