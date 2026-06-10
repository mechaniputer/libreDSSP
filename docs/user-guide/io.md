# Input and Output in DSSP
This page focuses on interactive I/O.

In the future, additional pages will exist for File I/O and other varieties of IO.

I am planning to introduce new words for safer and more convenient input/output.
For now, we support the following original DSSP IO words.


## Print Statements
The simplest way to display a literal string is with a print statement.
With this method, the content of the string must be defined in the source code (or shell input) prior to execution.
```
* ."Hello, world!"
Hello, Word!*
```

Notice that no newline was inserted, and the subsequent shell prompt was shown on the same line as the printed output.
If you need to terminate a printed line, you should use `CR` after the print statement:
```
* ."Hello, world!" CR
Hello, Word!
*
```

Similarly, `SP` will print a space:
```
* ."Hello," SP ."World!" CR
Hello, Word!
*
```

## Storing strings in memory

One way that you can place a string in memory is by entering a string literal with quotes:
```
* "foo bar baz"
* ..
[93947397451360 11]
*
```
The first value pushed to the stack is the memory address where the string's buffer is located.
The second value (`11`) is the length of the string.

Another way to place a string into memory is to use one of the **currently unimplemented** words to read data into a memory buffer (eg `TIS`, `TIB`), or read data from a file **(also unimplemented)**, or manipulate buffer contents in memory to construct a string **(You guessed it, unimplemented)**.


## Printing a string from memory
The `TOS` word is used to print a string located at some address in memory.
It takes two stack parameters, the same ones pushed when we entered a string in the previous section (address, length).
Both parameters are consumed.

Therefore, continuing from above, we print the string:
```
* ..
[93947397451360 11]
* TOS CR
foo bar baz
*
```

## Getting numeric input from the user
The `TIN` word reads numeric input from the user. `TIN` requires a size parameter on top of the stack, which will be consumed.
The size specifies how many digits the user should be allowed to type.

Example of correct usage:
```
* ."Enter a 2-digit number: " 2 TIN
Enter a 2-digit number: 42
* ..
[42]
*
```

If the user enters more than the specified number of digits, the number will be truncated, preserving the lowest signifigance digits.

Example of truncation:
```
* ."Enter a 2-digit number: " 2 TIN
Enter a 2-digit number: 123456
* ..
[56]
*
```

## Printing numeric data from the stack
The `TON` word prints a number out to some specificed number of digits.
It requires two stack operands, with the top being the number of digits to print, and the subtop the value to be printed.
Both stack operands will be consumed.

A newline is not printed automatically, so remember to use CR if you want a newline.

Example:
```
* 123456 6 TON CR
123456
*
```

If the number contains more than the specified number of digits, the number will be truncated, preserving the lowest signifigance digits.


Example with truncation:
```
* 123456 3 TON CR
456
*
```

You can of course use the word `.` to print the top of the stack with a newline, without truncation, and without consuming any stack values:
```
* 123456 .
123456
* ..
[123456]
```
