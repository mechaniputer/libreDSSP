# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

libreDSSP (libre Dialog System for Structured Programming) is a free software interpreter for the DSSP language. This language originated in the Soviet Union and was designed for the Setun ternary computer. It is very similar to Forth but has more compact syntax along with arguably more consistent rules regarding the behavior of stack operations and types. This implementation is not yet complete, but it is complete enough to experiment with. Work has resumed after a long delay but it will continue to be sporadic due to my busy schedule.

To our knowledge this is the only maintained DSSP implementation, and the first to be free software. It is lacking in many areas but progress is being made to add features and make it faster.

The ultimate goal of this project is not to merely replicate DSSP as it once existed, but to improve on it where possible without betraying the philosophy of the language.

## Getting started
Run 'make' to build it. You may wish to use the included tutorial by running './dssp TUTOR.DSP'.
Vim source highlighting files are included in vim/.

## Contributing
Pull requests with good style that solve actual problems are appreciated. When in doubt check the issues tab and feel free to comment for clarification or advice. If you find a way to crash the interpreter, make an issue. You may want to search for documents about prior implementations of DSSP. Most of them are in Russian which makes it difficult to get a full understanding of the language, and there have also been several dialects of DSSP. This project does not aim to precisely match any particular dialect but aims to have a high degree of compatibility with most of them.

The libreDSSP tutor (TUTOR.DSP) does not yet cover all of the implemented language features. Pull requests to add or improve training steps are appreciated.

Let me know if you are interested in setting up other resources such as an irc channel or a website. I don't currently have the time to manage anything like that.

## What works
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

## What doesn't work yet
	* SAVE, LOAD
	* ONLY, CANCEL, FORGET, CLEAR
	* Variable addresses using '
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
	* SHL, SHR, other bitwise operations
	* Everything unaccounted for in this README

## Possible future goals
	* Full documentation and tutorial
	* Floating point math
	* DSSP Compiler
	* Multithreading support
