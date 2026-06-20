# Variables

## A note on data sizes
In libreDSSP, all variables are sized to fit one [cell](jargon.md#cell-size).
On a 64-bit machine, variables are 64-bits. On a 32-bit machine, variables are 32 bits.

**NOTE: The DSSP language includes words that allow for more flexibility in data size, but libreDSSP currently lacks these words. These will be added in an upcoming version. The `BYTE` (`BIT`) prefix will declare a variable that always truncates to one byte (bit). `VCTR` and `ARR` will allow for defining larger contiguous sequences of these element types.**

## Declaring variables
Declaring a variable results in space for that variable being allocated and initialized to `0`.
A variable cannot have a name that is the same as any [currently-accessible](dictionaries.md) word or variable.

To declare a variable, use the `VAR` core word like so:
```
* VAR X
```
Variables must be declared before they can be assigned a value or referenced.
Note that a VAR statement will not be evaluated until a newline occurs, so you cannot do this:
```
* VAR X 5 ! X
```
Instead you can do this:
```
* VAR X
* 5 ! X
```

Support for compiling references to undeclared variables may be supported in a future version.
The best style approach, however, is to declare your variables at the top of the source file above any variable references.

## Assigning variables
To assign the top stack cell to a previously declared variable, use the `!` core word like so:
```
* ! X
```
You can reuse `!` to assign a new value to a variable at any point in the code.

## Recalling variables
Typing the name of a variable will result in the current value being pushed to the stack:
```
* VAR X
* 5 ! X
* X ..
[5]
* 
```

## Viewing defined variables
You can get a list of all defined variables across all [subdictionaries](dictionaries.md) with the `VARS` command.
To print the current value of a variable, use `SEE`.

Example:
```
* VARS
Subdict: $DEFAULT
  z  y  x
* SEE x
Subdict: $DEFAULT (OPEN)
  x: 7
*
```

## Deleting variables
You can use `DEL` to delete the a variable from the [subdictionary](dictionaries.md) currently selected for modification.
This is the only way to redefine a variable as a word.

## Shortcuts for altering variable contents
A future version of libreDSSP will add support for the following words: `!0, !1, !1-, !1+, !-, !+, !!!`. They will be documented here at that time.

## A note about subdictionaries
When you define a variable, it will go into whatever subdictionary is selected for modification. By default, this will be a subdictionary called `$DEFAULT`.
Words from one subdictionary can reference and use variables from any other subdictionary, but this can also be restricted.
For more about the use of subdictionaries in libreDSSP, refer to the [relevant page](dictionaries.md).