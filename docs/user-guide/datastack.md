# The Data Stack

libreDSSP is a so-called "stack-based language". A stack is directly visible and manipulable to the programmer, and is normally used for passing parameters and storing temporary data.

## Examining the stack
To view the stack, you can run the `..` word from the interactive prompt. Note that this word is also available for use in source programs. The stack is printed with bottom at the left end and the top of the stack at the right end. A newline character is also printed.

For example, the stack below has a `1` on the bottom, and a `3` on top.

```
[1 2 3]
```

You can use the word `.` to view only the top element of the stack. A newline is also printed. If the stack is empty, it will print  `(NIL)`.

## Pushing cells to the stack
In libreDSSP code, integer constants are normally compiled as a word that pushes that constant to the data stack. New cells are always placed on top of any prior elements.

For example, the line `3 6 9 12' pushes those cells to the stack in order from left to right, leaving 3 at the bottom and 12 at the top:
```
* 3 6 9 12
* ..
[3 6 9 12]
```

## Removing cells from the stack
You can remove (pop) the top element using the word `D`.

You can clear all cells from the stack using the word `DS`.

## Copying stack cells
You can duplicate the cell at the top of the stack using the word `C`.

If you want to copy a deeper cell to the top of the stack, you may use `C2`, `C3`, or `C4` depending on the depth of the cell you want to copy.

If you need to copy a cell from deeper than 4 cells down, there is a `CT` command for this purpose. `CT` pops the top cell from the stack and uses it as a depth parameter. For example, `10 CT` will copy a cell from 10 cells deep in the stack onto the top of the stack. Note that, in effect, the copied cell will replace the just-pushed depth parameter `10` on the stack. Also note that `1 CT` is equivalent to `C`.

## Exchanging stack cells
`E2` will swap the top two stack cells with each other. `E3` and `E4` swap deeper cells with the top cell. Similar to above, a command `ET` exists to exchange the top cell of the stack with a different cell of any specified depth. Note that like `CT`, `ET` likewise pops a cell from the stack to use as a depth parameter, and therefore will treat the cell below that as the top when performing the swap. Also note that `1 ET` does nothing, since it exchanges the top with itself.

## Measuring the stack depth
The word `DEEP` pushes the number of cells currently in the stack to the top of the stack. Note that although the resulting cell will now be off by 1, it will be correct again once the depth parameter has been popped for use by a word that needs to know the depth of the stack.

Regardless of stack depth, you can swap the top and bottom cells by `DEEP ET`, or copy the bottom cell to the top with `DEEP CT`.
