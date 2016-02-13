# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

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
	* [comments]
	* GNU readline support

## What doesn't work yet:
	* File IO
	* Multiple subvocabularies
	* Variables, arrays, fixed variables,etc
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
	* DSSP Compiler
