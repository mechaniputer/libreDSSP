# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

libreDSSP (libre Dialog System for Structured Programming) is my attempt to write an interpreter for a very obscure language similar to Forth. This language originated in the Soviet Union and was designed for the Setun ternary computer.

To my knowledge this is the only maintained DSSP implementation, and the first to be free software. It is lacking in many areas but progress is being made to add features and make it faster.

The ultimate goal of this project is not to merely replicate DSSP as it once existed, but to improve on it where possible without betraying the philosophy of the language.

Pull requests to solve bugs or improve style are welcome.

## Getting started
Run 'make' to build it. You may wish to use the included tutorial by running './dssp TUTOR.DSP'.
Vim source highlighting files are included in vim/.

## What works:
	* Basic math operations (+,*,-,/)
	* NEG, ABS
	* BYE, ..(show stack), .(show top of stack)
	* IF+, IF0, IF-
	* BR+, BR0, BR-, BRS, BR
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
