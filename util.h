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

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined(HAVE_LIBEDIT)
#	if defined(__NetBSD__)
#	include <readline/readline.h>
#	include <readline/history.h>
#	elif defined(__APPLE__) || defined(__FreeBSD__)
#	include <editline/readline.h>
#	else // Linux/other
#	include <editline/readline.h>
#	include <editline/history.h>
#	endif
#elif defined(HAVE_READLINE)
// Standard GNU Readline path
#  include <readline/readline.h>
#  include <readline/history.h>
#else
#	error "No command-line editing library defined!"
#endif

#include "dict.h"
#include "stack.h"

void abortExecution(void);
void debug();
void print_codewords(codeword_t ** array);
int isNum(char * foo);
void word_next();
void word_enter();
void word_exit();
char * prompt(int ready);

#endif
