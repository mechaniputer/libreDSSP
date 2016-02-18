# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

libreDSSP (libre Dialog System for Structured Programming) is my attempt to write an interpreter for a very obscure language similar to Forth. This language originated in the Soviet Union and was designed for the Setun ternary computer.

This is a work in progress. It isn't complete enough to be very useful yet.

## What works:
	* Basic math operations (+,*,-,/)
	* NEG, ABS
	* BYE, ..(show stack), .(show top of stack)
	* IF+, IF0, IF-
	* DO
	* D (drop), C (copy), DS (drop entire stack)
	* DEEP (push height of stack)
	* ET, E2, E3, E4
	* CT, C2, C3, C4
	* Function declarations
	* GROW, USE, SHUT, ?$
	* Integer variables
	* TIN, TON
	* ."hello" printing, SP, CR
	* [comments]
	* GNU readline support
	* Read from file at start

## What doesn't work yet:
	* SAVE, LOAD
	* ONLY, CANCEL, FORGET, CLEAR
	* Variable addresses
	* Arrays, fixed variables, etc
	* BR+, BR0, BR-, BRS, BR
	* RP
	* SGN, NOT
	* EX, EX-, EX0, EX+, EXT
	* SORT, SPIN, MAX, MIN
	* 1+, 2+, 1-, 2-, etc
	* T0, T1
	* TRB, TOB, BASE@
	* TIB, TIS
	* B2, B8, B10, B16
	* Various bitwise operations
	* Everything unaccounted for in this README

## Possible future goals:
	* Full documentation and tutorial
	* Floating point math
	* DSSP Compiler
