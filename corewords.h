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

#ifndef COREWORDS_H
#define COREWORDS_H

#include <stdlib.h>
#include "stack.h"
#include "cmdbuf.h"

extern stack *dataStack;
extern cmdbuffer *cmdbuf;
extern stack *returnStack;
extern codeword_t *current_codeword;

typedef struct dict dict;

// Arithmetic
void push_zero();
void push_one();
void push_two();
void push_four();
void push_eight();
void plus();
void multiply();
void minus();
void divide();
void negate();
void absval();
void sign();
void max();
void min();

void plus1();
void plus2();
void plus3();
void plus4();

void minus1();
void minus2();
void minus3();
void minus4();

// Display and interpreter
void bye();
void showTop();
void showStack();
void base10();

// Conditionals
void ifplus();
void ifzero();
void ifminus();
void branchminus();
void branchzero();
void branchplus();
void branchsign();
void branch();
void equality();
void greaterthan();
void lessthan();

// Looping
void do_begin();
void do_loop();
void rp_begin();
void rp_loop();
void loop_exit();
void loop_exit_minus();
void loop_exit_zero();
void loop_exit_plus();
void loop_exit_nested();

// Stack manipulation
void exch2();
void exch3();
void exch4();
void exchdepth();
void copy();
void copy2();
void copy3();
void copy4();
void copydepth();
void drop();
void dropStack();

// Misc
void skip1();
void skip2();
void noop();
void _undefined_ref();
void _var_undef_assign();
void delete_name();
void pushLiteral();
void _var8_ref();
void declare_var8();
void _var_generic_assign();
void _var8_ref();
void printNewline();
void printSpace();
void listDicts();
void growSub();
void shutSub();
void openSub();
void termInNum();
void termOutNum();
void termOutString();
void stackDepth();
void inventoryUndefined();
void inventoryWords();
void inventoryVars();
void inventoryVars();
void seeName();

// Add all of the core words to a dictionary
void define_all_core();

#endif
