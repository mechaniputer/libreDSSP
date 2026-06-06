# Arithmetic in libreDSSP
This page covers integer arithmetic. libreDSSP does not yet support other kinds of arithmetic.

## Addition
Addition consumes two cells from the top of the stack and then pushes a cell containing the result.
To add the top two values of the stack together, use the word `+`.

Example:
```
* ..
[6 7]
* +
* ..
[13]
```

## Subtraction
Subtraction consumes two cells from the top of the stack and then pushes a cell containing the result.
To subtract the top of the stack from the value below it, use the word `-`.

Example:
```
* ..
[17 23]
* -
* ..
[-6]
```
Note that the result in this example is negative. By default, libreDSSP treats cells as signed values. On currently supported platforms this relies on two's complement binary notation.

## Multiplication
Multiplication consumes two cells from the top of the stack and then pushes a cell containing the result.
To multiply the top two values of the stack, use the word `*`.

Example:
```
* ..
[15 6]
* *
* ..
[90]
```

## Division
Division consumes two cells from the top of the stack and then pushes two cells containing the results.
**Important: when performing integer division, libreDSSP produces not one, but two results.**
The quotient is pushed first, followed by the remainder.

To divide the cell immediately below the top of the stack by the top cell, use the word `/`.

Example:
```
* ..
[100 33]
* /
* ..
[3 1]
```

## Negation
To negate the sign of the top cell, use the word `NEG`.
The top cell will be replaced.
This is equivalent to `-1 *`.

## Absolute value
To take the absolute value of the top cell, use the word `ABS`.
The top cell will be replaced.
This is equivalent to `C IF- NEG`.

## Shorthand addition/subtraction
libreDSSP includes several words to perform common increment/decrement operations.

The reason for the existence of these words is threefold:

1. They can make code nicer to read.

2. They can yield better performance and smaller code compared to performing the same operation using two separate words and a literal cell.

3. The DSSP language places some intentional restrictions on code structure. One such restriction is that conditional, branch and loop operands must consist of a single word. If you need to conditionally increment a value, these words can come in handy.

To add 1 to the top of the stack, you can use `1+`. The words `2+`, `3+`, and `4+` are likewise available.

To subtract 1 from the top of the stack, you can use `1-`. The words `2-`, `3-`, and `4-` are likewise available.

## Integer comparisons

If the top two stack cells are equal, the word `=` will consume both cells and push `1`. Otherise it will consume both cells and push `0`.

Example:
```
* ..
[2 3 3]
* =
* ..
[2 1]
* =
* ..
[0]
```

The word `<` compares and consumes the top two cells of the stack. If the lower cell was less than the top cell, then a 1 is pushed. Otherwise a 0 is pushed.

Example:
```
* ..
[3 4]
* <
* ..
[1]
```

The word `>` compares and consumes the top two cells of the stack. If the lower cell was greater than the top cell, then a 1 is pushed. Otherwise a 0 is pushed.

Example:
```
* ..
[3 4]
* >
* ..
[0]
```