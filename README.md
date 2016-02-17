# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

libreDSSP (libre Dialog System for Structured Programming) is my attempt to write an interpreter for a very obscure language similar to Forth. This language originated in the Soviet Union and was designed for the Setun ternary computer.

This is a work in progress. It isn't complete enough to be very useful yet.

## What works:
	* Basic math operations (+,*,-,/)
	* NEG, ABS
	* BYE, ".."(show stack), "."(show top of stack)
	* IF+, IF0, IF-
	* DO
	* "D"(drop), "C"(copy)
	* ET, E2, E3, E4, etc
	* CT, C2, C3, C4, etc
	* Function declarations
	* Integer variables
	* [comments]
	* GNU readline support

## What doesn't work yet:
	* File IO
	* Multiple subvocabularies
	* Variable addresses
	* Arrays, fixed variables, etc
	* BR+, BR0, BR-, BRS, BR
	* RP
	* EX, EX-, EX0, EX+, EXT
	* SORT, SPIN, MAX, MIN
	* 1+, 2+, 1-, 2-, etc
	* T0, T1, etc
	* TRB, TOB, TON, BASE@
	* TIB, TIN, TIS
	* ."hello" printing, SP, CR
	* B2, B8, B10, B16
	* Various bitwise operations
	* Everything unaccounted for in this README

## Possible future goals:
	* Full documentation and tutorial
	* Floating point math
	* DSSP Compiler
