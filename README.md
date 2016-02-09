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
	* Function declarations
	* [comments]

## What doesn't work yet:
	* GNU readline support
	* File IO
	* Multiple subvocabularies
	* Variables, arrays, fixed variables,etc
	* BR+, BR0, BR-, BRS, BR
	* RP
	* EX, EX-, EX0, EX+, EXT
	* TRB, TOB, TON, BASE@
	* TIB, TIN, TIS
	* ."hello" printing, SP, CR
	* B2, B8, B10, B16
	* Everything unaccounted for in this README

## Possible future goals:
	* DSSP Compiler
