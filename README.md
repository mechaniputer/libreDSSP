# libreDSSP
## A GPLv3 Licensed DSSP Interpreter

libreDSSP (libre Dialog System for Structured Programming) is a free software interpreter for the DSSP language. This language originated in the Soviet Union and was inspired by the architecture of the Setun ternary computer. It is very similar to Forth but has greatly simplified "mode" semantics, more compact syntax, and arguably more consistent rules regarding the behavior of stack operations and the evaluation of variables and addresses. DSSP also supports top-down programming, meaning that as-yet undefined words can be referenced in other words.

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
The [Official libreDSSP Docs](https://mechaniputer.github.io/libreDSSP/) are still incomplete, but the basics are covered.

In addition some code examples are provided in the examples directory, and Vim source highlighting files are included in vim/.


## Contributing
Pull requests with good style that solve actual problems are appreciated. When in doubt check the issues tab and feel free to comment for clarification or advice. If you find a way to crash the interpreter, make an issue with instructions (ideally DSSP code) to reproduce it.

Before contributing you may want to search for documents about prior implementations of DSSP. Documentation is scarce, is often fragmentary, and is usually written in Russian. To add confusion, there have also been several dialects of DSSP with various differences. This project does not aim to precisely match any particular dialect but aims to have a high degree of compatibility with most of them. Note that we are not pursuing ternary support and only target 64-bit and 32-bit hardware.

Contributions should preserve DSSP syntax, behavior, and approach, rather than simply following what is usually done in FORTH (although an understanding of FORTH can help fill gaps in information about DSSP).

Also let us know if you are interested in setting up other resources such as an irc channel or mailing list. We don't currently have the time to manage anything officially, but if someone else sets one up, we can link to it here on an unofficial basis.

## What is different from earlier DSSP dialects?
- libreDSSP supports 64-bit cells when running on 64-bit platforms. We are not aware of any prior DSSP implementations with 64-bit cells. Prior systems were either 16-bit or 32-bit (we also support the latter).

- The $PRIME subdictionary contains core words and cannot be modified. Instead, a $DEFAULT subdictionary exists for user-defined words, and more subdictionaries can be added.

- The core word WORDS displays an inventory of all user-defined words in open subdictionaries. This is a loanword from Forth that may or may not have had an equivalent in past dialects of DSSP.


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
- VCTR, ARR, CNST, BYTE, WORD, LONG, QUAD (new), VALUE, TEXT, FIX, EMPTY, EQU
- ' (push address of var)
- '' (push address of function)
- @ (dereference top of stack and push result)
- ACT (mark a VAR as an executable word pointer, callable by name)
- !T, !TB, etc (dereference top of stack and store the 2nd stack operand)
- T0, T1 (assign 0 or 1 to prior top of stack)
- SGN (push -1, 0, or 1 based on sign of stack top)
- MAX, MIN
- Variable/Array store shortcuts: !0, !1, !1-, !1+, !-, !+, !!!
- Bitwise: SHL, SHR, NOT, INV, &, &0, '+', SHT
- Word ops: SWB, SWW, LO, HI, SETHI, SETLO, SGX (Note: These will have to be augmented for 64-bit words)
- ONLY, CANCEL, FORGET, CLEAR
- :: (to define words not subject to CLEAR)
- DEFINE? (Predicate for whether a word is defined. I assume this should not check in CLOSED subdicts.)
- SAVE, LOAD (for saving state to, or restoring state from, a source file)
- TRB, TOB
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


## Licensing
All libreDSSP code, all libreDSSP example programs, all libreDSSP markdown documentation, the vim source highlighting configuration files, and all custom files that configure and format the documentation, are licensed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html).

[Properdocs](https://properdocs.org/) (the tool we use to prepare our documentation) is under the [MkDocs License (BSD)](https://properdocs.org/about/license/).

## AI Policy
The libreDSSP project prioritizes authenticity over perfection. None of the documentation was AI-generated, and significant code in libreDSSP itself was AI-generated. At present, the only really AI-generated code in libreDSSP is found in [dssp.js](https://github.com/mechaniputer/libreDSSP/blob/master/docs/dssp.js), which handles DSSP source highlighting on the official docs (obviously highlight.js doesn't know about DSSP). Even this file was generated with many rounds of human input and substantial manual revision, making it eligible for copyright (AI tools know practically nothing that is accurate about DSSP anyway, although the existence of these docs might change that soon).

Aside from the above, I have used AI assistance when working on libreDSSP for the following tasks:

- Taking inventory of the project state when I resumed work after a multi-year delay and had forgotten what I was doing nearly 7 years prior.
- Translation of original Russian DSSP documents and papers to help me learn DSSP syntax.
- Planning the addition of new features and to reduce the risk of forgetting to apply changes to older code.
- Occasionally, AI auto-completion was used during repetitive or tedious refactoring.

In all of the above, all AI output was taken with more than a few grains of salt and everything was also rechecked in other ways. The AI tools were just one of many data points so that they added to, rather than harmed, the likelihood of correctness.