# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

libreDSSP (libre Dialog System for Structured Programming) is a free software interpreter for the DSSP language. This language originated in the Soviet Union and was designed for the Setun ternary computer. It is very similar to Forth but has more compact syntax along with arguably more consistent rules regarding the behavior of stack operations and the evaluation of variables and addresses. DSSP also supports top-down programming, meaning that as-yet undefined words can be referenced in other words.

To our knowledge this is the only maintained DSSP implementation, and the first to be free software (GPLv3 license).

libreDSSP uses an indirect threaded design and avoids any inline assembly for maximum portability. DSSP provides a somewhat larger set of core words than most FORTH-style languages. We consider this a strength, because memory is abundant on modern platforms (at least relative to our needs). Having more complex core words also eases development and improves performance by leveraging compiler optimizations.

## Getting started
Run 'make' to build it. You may wish to use the included tutorial by running './dssp examples/TUTOR.DSP'.
Vim source highlighting files are included in vim/.

## Contributing
Pull requests with good style that solve actual problems are appreciated. When in doubt check the issues tab and feel free to comment for clarification or advice. If you find a way to crash the interpreter, make an issue with instructions to reproduce it.

Before contributing you may want to search for documents about prior implementations of DSSP. Documentation is scarce, is often fragmentary, and is usually written in Russian. To add confusion, there have also been several dialects of DSSP with various differences. This project does not aim to precisely match any particular dialect but aims to have a high degree of compatibility with most of them. Note that we are not pursuing ternary support and only target 64-bit and 32-bit hardware.

Contributions should preserve DSSP syntax, behavior, and approach, rather than simply following what is usually done in FORTH (although an understanding of FORTH can help fill gaps in information about DSSP).

The libreDSSP tutor (TUTOR.DSP) does not yet cover all of the implemented language features. Pull requests to add or improve training steps are appreciated.

Also let us know if you are interested in setting up other resources such as an irc channel or a website. We don't currently have the time to manage anything officially, but if someone sets one up, we can link to it here.

## What is different from earlier DSSP dialects?
The $PRIME subdictionary contains core words and cannot be modified. Instead, a $DEFAULT subdictionary exists for user-defined words, and more subdictionaries can be added.

In addition, WORDS lists an inventory of all user-defined words in open subdictionaries. This is a loanword from FORTH that may or may not have had an equivalent in past dialects of DSSP.


## What works
- Top-down programming! (reference undefined words when defining words)
- UNDEF (list undefined words)
- Basic math operations (+,*,-,/)
- VAR, !, push value of variable by name
- 1+, 2+, 3+, 4+, 1-, 2-, 3-, 4-
- =, <, >
- NEG, ABS
- BYE, ..(show stack), .(show top of stack)
- DO
- IF+, IF0, IF-
- BR+, BR0, BR-, BRS, BR
- D (drop), C (copy), DS (drop entire stack)
- DEEP (push height of stack)
- ET, E2, E3, E4
- CT, C2, C3, C4
- Function declarations
- TOS
- TIN, TON (Not sure if correct behavior)
- SP, CR
- ."hello" printing
- [comments]
- Editline support (Also works with GNU readline)
- Load and run code from file at startup
- B10 (as a placeholder since we currently only support base 10 I/O)
- GROW, USE, SHUT, ?$
- WORDS (This is a FORTH loanword. Currently unsure if DSSP had an equivalent.)


## What doesn't work yet
- Push address of string
- SAVE, LOAD
- ONLY, CANCEL, FORGET, CLEAR
- Arrays, fixed variables, etc
- RP, EX, EX-, EX0, EX+, EXT
- SGN, NOT
- SORT, SPIN, MAX, MIN
- T0, T1
- TRB, TOB, BASE@
- TIB, TIS
- B2, B8, B16
- SHL, SHR, other bitwise operations
- ' (push address of var)
- '' (push address of function)
- @ (dereference top of stack and push result)
- !T, !TB, etc (dereference top of stack and store the 2nd stack operand)
- EXEC (execute function from address on stack)
- TEXEC (execute text)
- BELL
- LPSP, LPCR, LPS, LPT, LPFF
- INT, TRAP, ON, EON
- Everything unaccounted for in this README


## Possible future goals
- Full documentation and in-shell interactive tutorial
- Floating point math
- External libraries for special or platform-specific uses
- Portability improvements including embedded devices
- Multithreading support by spawning additional kernels?
