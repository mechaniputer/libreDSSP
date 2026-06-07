# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

libreDSSP (libre Dialog System for Structured Programming) is a free software interpreter for the DSSP language. This language originated in the Soviet Union and was inspired by the architecture of the Setun ternary computer. It is very similar to Forth but has greatly simplified "mode" semantics, more compact syntax, and arguably more consistent rules regarding the behavior of stack operations and the evaluation of variables and addresses. DSSP also supports top-down programming, meaning that as-yet undefined words can be referenced in other words.

DSSP provides a somewhat larger set of core words than most FORTH-style languages. We consider this a strength, because memory is abundant on modern platforms (at least relative to our needs). Having more specific core words also eases development and improves performance.

To our knowledge this is the only maintained DSSP implementation, and the first to be free software (GPLv3 license).

libreDSSP uses an indirect threaded design and avoids any inline assembly for maximum portability. At the moment we target every modern *nix system that we reasonably can, but most are untested. Longer term, embedded ports would certainly be nice to have.

## Getting started
This project uses a lightweight, portable configuration framework that automatically detects and binds to either `libedit` (Editline) or GNU `readline`.
Ensure you have a standard C compiler (`cc`, `gcc`, or `clang`), `make`, and one of the aforementioned editing libraries installed. The configure script prefers `libedit` by default since it results in lower memory usage at runtime.

### Compilation Steps

To build the executable, run the standard configuration and build sequence in your terminal:

```bash
./configure
make
```

### Build Targets

* `make` — Compiles the standard optimized production binary (`dssp`).
* `make debug` — Compiles a debug version containing symbols (`-ggdb`) and activates AddressSanitizer (`-fsanitize=address`) for memory debugging and leak checking.
* `make clean` — Removes generated binaries, object files, and temporary configurations to reset the build directory.


### How to use libreDSSP
The [Official libreDSSP Docs](https://mechaniputer.github.io/libreDSSP/) are still incomplete, but are probably helpful for new users.

In additon some code examples are provided in the examples directory, and Vim source highlighting files are included in vim/.


## Contributing
Pull requests with good style that solve actual problems are appreciated. When in doubt check the issues tab and feel free to comment for clarification or advice. If you find a way to crash the interpreter, make an issue with instructions (ideally DSSP code) to reproduce it.

Before contributing you may want to search for documents about prior implementations of DSSP. Documentation is scarce, is often fragmentary, and is usually written in Russian. To add confusion, there have also been several dialects of DSSP with various differences. This project does not aim to precisely match any particular dialect but aims to have a high degree of compatibility with most of them. Note that we are not pursuing ternary support and only target 64-bit and 32-bit hardware.

Contributions should preserve DSSP syntax, behavior, and approach, rather than simply following what is usually done in FORTH (although an understanding of FORTH can help fill gaps in information about DSSP).

Also let us know if you are interested in setting up other resources such as an irc channel or mailing list. We don't currently have the time to manage anything officially, but if someone sets one up, we can link to it here on an unofficial basis.

## What is different from earlier DSSP dialects?
First of all, it supports 64-bit cells when running on 64-bit platforms. We are not aware of any prior DSSP implementations with 64-bit cells. Prior systems were either 16-bit or 32-bit (we also support the latter).

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
- ..(show stack), .(show top of stack)
- IF+, IF0, IF-
- BR+, BR0, BR-, BRS, BR
- DO, RP (loops)
- EX (loop exit), EX-, EX0, EX+, EXT (nested loop exit)
- D (drop), C (copy), DS (drop entire stack)
- DEEP (push height of stack)
- ET, E2, E3, E4
- CT, C2, C3, C4
- Function declarations
- TOS
- TIN, TON (Not sure if correct behavior)
- SP, CR
- ."hello" printing
- Push address and len of string literal
- [comments]
- Editline support (Also works with GNU readline)
- Load and run code from file at startup
- B10 (as a placeholder since we currently only support base 10 I/O)
- GROW, USE, SHUT, ?$
- WORDS (This is a FORTH loanword. Currently unsure if DSSP had an equivalent.)
- BYE


## What doesn't work yet
- File IO (this doesn't seem to be very standardized so we are allowed to make good choices here)
- References to undefined vars
- Arrays, fixed variables, etc
- ' (push address of var)
- '' (push address of function)
- @ (dereference top of stack and push result)
- !T, !TB, etc (dereference top of stack and store the 2nd stack operand)
- SHL, SHR, other bitwise operations
- ONLY, CANCEL, FORGET, CLEAR
- SAVE, LOAD
- SGN, NOT
- SORT, SPIN, MAX, MIN
- T0, T1
- TRB, TOB, BASE@
- TIB, TIS
- B2, B8, B16
- EXEC (execute function from address on stack)
- TEXEC (execute text)
- BELL
- LPSP, LPCR, LPS, LPT, LPFF
- INT, TRAP, ON, EON
- Floating point math (not an original DSSP feature)
- Everything unaccounted for in this README


## Possible future goals
- Libraries for various algorithms, data structures, etc.
- Platform-specific libraries for special uses (graphics, networking, GPIO)
- Ports to additional OSes and embedded devices
- Multithreading support, callbacks
- Ability to generate tiny standalone executables
