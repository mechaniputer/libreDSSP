# libreDSSP Core Words

libreDSSP provides a set of built-in "core" words. These words cannot be removed or redefined once libreDSSP has been built. All other libreDSSP programs consist of concatenations of these core words.

The table below summarizes core words that are meant to be used in libreDSSP code and/or the interactive prompt. Currently there are some other core words "under the hood" that are not meant to be used explicitly. Those are not documented here, and a future version of libreDSSP will make them inaccessible anyway.

As work on libreDSSP progresses, the table will get longer. Refer to the list of non-working features on our [GitHub repository](https://github.com/mechaniputer/libreDSSP).

The order is mostly alphabetical with some minor variances for the purpose of grouping. I will probably introduce organized sections later.

**Obviously, the table isn't done yet. I think has all of the rows at least.**

| Core word | Description | Example |
|---|---|---|
| + | Add | |
| - | Subtract | |
| * | Multiply | |
| / | Divide | |
| < | Less than | |
| > | Greater than | |
| = | Equal | |
| . | Show top of stack | |
| .. | Show entire stack | |
| ! | Assign top of stack to an existing variable | |
| ?$ | List available subdictionaries | |
| 0 | Push 0 | |
| 1 | Push 1 | |
| 2 | Push 2 | |
| 4 | Push 4 | |
| 8 | Push 8 | |
| 1+ | Add 1 to top of stack | |
| 1- | Subtract 1 from top of stack | |
| 2+ | Add 2 to top of stack | |
| 2- | Subtract 2 from top of stack | |
| 3+ | Add 3 to top of stack | |
| 3- | Subtract 3 from top of stack | |
| 4+ | Add 4 to top of stack | |
| 4- | Subtract 4 from top of stack | |
| ABS | Take absolute value of top of stack | |
| B10 | Set mode to Base 10 arithmetic (placeholder) | |
| BR | Multi-clause branch | |
| BR+ | Branch on positive | |
| BR0 | Branch on zero | |
| BR- | Branch on negative | |
| BRS | Three-way branch on sign | |
| BYE | Exit libreDSSP | |
| C | Duplicate top of stack | |
| C2 | Copy cell from depth 2 to top of stack | |
| C3 | Copy cell from depth 3 to top of stack | |
| C4 | Copy cell from depth 4 to top of stack | |
| CT | Consume top stack cell and copy the cell from given depth to top of stack | |
| CR | Print a newline | |
| D | Drop the top cell from the stack | |
| DEEP | Push the stack depth to the stack | |
| DEL | Delete a named object (globally patches all refs to safe errors) | |
| DO | Loop a word a given number of times | |
| DS | Drop the entire stack contents | |
| E2 | Exchange the top stack cell with the cell below it | |
| E3 | Exchange the top stack cell with the cell at depth 3 | |
| E4 | Exchange the top stack cell with the cell at depth 4 | |
| ET | Consume top stack cell and exchange new top cell with the cell at given depth | |
| EX | Exit from the current innermost loop | |
| EX+ | Consume the top stack cell and if it is positive, exit from the current innermost loop | |
| EX0 | Consume the top stack cell and if it is zero, exit from the current innermost loop | |
| EX- | Consume the top stack cell and if it is positive, exit from the current innermost loop | |
| EXT | Consume the top stack cell and exit from that many layers of ongoing loops | |
| GROW | Select a subdictionary for expansion | |
| IF+ | Consume top stack cell; If the value is positive, execute the following word; Otherwise skip it | |
| IF0 | Consume top stack cell; If the value is zero, execute the following word; Otherwise skip it| |
| IF- | Consume top stack cell; If the value is negative, execute the following word; Otherwise skip it | |
| NEG | Negate the cell at top of stack | |
| NOP | Do nothing (no-op)| |
| RP | Repeat the following word until a loop exit is triggered | |
| SHUT | Disable lookup access to a given subictionary | |
| SP | Print a space | |
| TIN | Terminal Input Number | |
| TON | Terminal Output Number | |
| TOS | Terminal Output String | |
| UNDEF | List undefined words | |
| USE | Enable lookup access to a given subdictionary | |
| VAR | Declare a new variable (init to 0) | |
| WORDS | List all user-defined words | |
