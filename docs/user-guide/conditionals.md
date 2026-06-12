# DSSP Conditionals

DSSP provides the following categories of conditional [core words](corewords.md).


## A note about permitted branch targets
All of the words described on this page require one or more branch targets (words to be conditionally executed).
In DSSP, branch targets must be single-word operands that compile into a single cell.
This means that the majority of core words and any user-defined word are permissible as branch targets.

The core words that are not allowed as branch targets are:
`IF-`,`IF0`,`IF+`,`BR-`,`BR0`,`BR+`,`BRS`,`BR`,`DO`,`RP`,`VAR`,`!`,`GROW`,`SHUT`,`USE`.

In addition, you may not use a [print statement](io.md#print-statements) as a branch target, nor may you use a literal value that is not also defined as a [core word](corewords.md).


## Readability and code structure
The single-cell restriction on branch targets encourages a modular code structure with intentional layers of word definitions.
Decomposing problems into smaller units can be challenging at first but the end result is more readable and maintainable.

For more about how to write maintainable code in DSSP, see the page about [code style](style.md)


## 1-target IF
DSSP provides three single-target IF-type words with similar behavior: `IF-`, `IF-`, and `IF+`.
These words will either skip or not skip the following word depending on the value at the top of [the stack](datastack.md).
Note that these words all consume the top stack value.


Example for `IF-`:
```
* : NTV ."The number is negative" CR ;
* 0 IF- NTV
* -1 IF- NTV
The number is negative
* 
```

Example for `IF0`:
```
* : ZER ."The number is zero" CR ;
* -7 IF0 ZER
* 0 IF0 ZER
The number is zero
*
```

Example for `IF+`:
```
* : POS ."The number is positive" CR ;
* 3 IF+ POS
The number is positive
* 0 IF+ POS
* 
```

## 2-target BR
DSSP provides three dual-target BR-type words with similar behavior: `BR-`, `BR0`, and `BR+`.
These words will execute either the following word or the one after that depending on the value at the top of [the stack](datastack.md).
Note that these words all consume the top stack value.


Example for `BR-`:
```
* : NTV ."The number is negative" CR ;
* : NON ."The number is not negative" CR ;
* 5 BR- NTV NON
The number is not negative
* -5 BR- NTV NON
The number is negative
* 
```

Example for `BR0`:
```
* : ZER ."The number is zero" CR ;
* : NON ."The number is not zero" CR ;
* 5 BR0 ZER NON
The number is not zero
* 0 BR0 ZER NON
The number is zero
* 
```

Example for `BR+`:
```
* : POS ."The number is positive" CR ;
* : NON ."The number is not positive" CR ;
* 5 BR+ POS NON
The number is positive
* 0 BR+ POS NON
The number is not positive
* 
```

## 3-target BR
DSSP provides one triple-target BR-type word, `BRS`.
`BRS` will execute one of the three following words depending on whether the top stack value is negative, zero, or positive.
Note that this word consumes the top stack value.


Example of BRS:
```
* : CHECKSIGN BRS NGT ZER POS ;
* : NGT ."The sign is negative" CR ;
* : ZER ."The number is 0" CR ;
* : POS ."The sign is positive" CR ;
* -7 CHECKSIGN
The sign is negative
* 0 CHECKSIGN
The number is 0
* 3 CHECKSIGN
The sign is positive
* 
```

## Many-target BR
DSSP provides one many-target BR-type word, `BR`.

`BR` requires that the words following `BR` be an alternating sequence of integer values and branch targets, terminated with an `ELSE` and one more branch target. See the example below for clarification.

`BR` will execute one of the subsequent words depending on which condition, if any, matches the value on top of the stack.
If none of the conditions match, the word following `ELSE` is executed instead.
Note that this word consumes the top stack value.

Example for BR:
```
* : NUMBER BR 1 ONE 2 TWO 3 THREE 4 FOUR ELSE OTHER ;
* : ONE ."The number is one!" CR ;
* : TWO ."The number is two!" CR ;
* : THREE ."The number is three!" CR ;
* : FOUR ."The number is four!" CR ;
* : OTHER ."Oops, that number is not in the range [1,4]" CR ;
* 1 NUMBER
The number is one!
* 4 NUMBER
The number is four!
* 42 NUMBER
Oops, that number is not in the range [1,4]
* 
```