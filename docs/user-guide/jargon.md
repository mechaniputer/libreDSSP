# Some libreDSSP Jargon

## libreDSSP words
One of the most important terms to understand in these docs is "word".
In libreDSSP (and Forth), the term "word" is used to refer to what other languages might consider a "function" or "subroutine".
libreDSSP consists of [core words](corewords.md) and allows programmers to define additional [user words](userwords.md).

## Cells
A cell is the basic unit of data in libreDSSP. The [data stack](datastack.md) contains 0 or more cells of information. A compiled DSSP word consists of an array of cells that contain references to other words. If there are no referenced-but-undefined words, then all of the word definitions form a directed (though not necessarily acyclic) graph where the sink nodes (aka leaves) are all [core words](corewords.md).


## Cell size
Unfortunately, the word "word" can also refer to the size of the machine word. In libreDSSP the machine word size determines the size of cells, so these docs will try to refer to the "cell size" rather than "word size". libreDSSP targets machines with a word size of either 32 or 64 bits, and the cell size will match the word size of the machine that libreDSSP was compiled for.

## Top-down programming
libreDSSP (unlike most Forth dialects) allows you to define a word that calls into another word that is not yet defined (including the very word that you are currently defining, as in recursion). This allows for "top-down programming", in which you break the problem into subproblems prior to implementing solutions to any of those subproblems.

This also makes it possible to test an incomplete program. If an undefined word is run during testing, execution will cleanly abort and return to the libreDSSP prompt.

You can view a list of all currently undefined words by running the word `UNDEF`.