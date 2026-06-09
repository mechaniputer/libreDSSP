# DSSP Loops
DSSP provides two types of loops, as well as several words that can be used to terminate a loop from within.

## DO loops
The DSSP `DO` loop is used when the number of repetitions (or the maximum number) is known in advance.

Example for `DO`:
```
* : HELLO ."Hello world!" CR ;
* 5 DO HELLO
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
* 
```

## RP Loops
The DSSP `RP` loop is used when a loop must continue indefinitely, or until some condition is met.

Example for `RP`:
```
* : HELLO ."Hello world!" CR ;
* RP HELLO
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
Hello world!
(...and so on...)
```

## Loop Exits
DSSP provides the following words to exit from a loop: `EX`, `EX-`, `EX0`, `EX+`, `EXT`

The simplest loop exit is `EX`. If executed, `EX` will unconditionally exit from a loop.
`EX` is normally combined with one of the [conditional words](conditionals.md), as in the example below:

```
* LOOP  : LOOP  RP BODY ;
* BODY  : BODY  1- C BR- DONE NOTDONE ;
* DONE  : DONE  ."Exiting loop!" CR EX ;
* NOTDONE  : NOTDONE  ."The value is " . CR ;
* 5 LOOP
The value is 4
The value is 3
The value is 2
The value is 1
The value is 0
Exiting loop!
* 
```

The words `EX-`, `EX0`, and `EX+` all work similarly to `EX`, except that the loop exit is done conditionally without relying on any other conditionals.

`EX-` will consume the top stack cell and exit the loop if the value was negative.
`EX0` will consume the top stack cell and exit the loop if the value was zero.
`EX+` will consume the top stack cell and exit the loop if the value was positive.


For example:
```
* : BODY 1- C EX- ."The value is " . ;
* : LOOP RP BODY ."Loop terminated!" CR ;
* 5 LOOP
The value is 4
The value is 3
The value is 2
The value is 1
The value is 0
Loop terminated!
```

Finally, `EXT` allows the program to (unconditionally) exit multiple layers of nested loops.
`EXT` consumes the top of the stack and exits that many layers of loops (they can be either `RP` loops, `DO` loops, or some interleaving of the two).
`EXT` is normally combined with one of the [conditional words](conditionals.md), as in the (admittedly complicated) example below:

```
* : FOOLOOP ."FOO" CR BAR ;
* : BAR RP BARLOOP ;
* : BARLOOP ."BAR" CR RP BAZ ;
* : BAZ ."BAZ" CR 1- C BR0 DONE EX ;
* : DONE ."Doing a nested exit." CR 3 EXT ;
* 3 RP FOOLOOP
FOO
BAR
BAZ
BAR
BAZ
BAR
BAZ
Doing a nested exit.
```
