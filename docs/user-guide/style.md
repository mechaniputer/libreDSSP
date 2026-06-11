# DSSP Coding Style

Taking correct syntax as a given, this page suggests some standard formatting practices to improve readability of libreDSSP code.

These guidelines are designed with the goal of producing readable code for libreDSSP.
Some of these practices might be incompatible with earlier DSSP implementations.

If these guidelines are not sufficient to specify what you need to, feel free to improvise since these guidelines are still being worked out.
Please [let us know](https://github.com/mechaniputer/libreDSSP/issues) if you think of any other useful guidelines to include here.


## Commenting
In DSSP, comments are enclosed by square brackets. Comments may span multiple lines:
```
[This is a comment]
[This
is a
multi-
line
comment]
```


## Code Organization
We recommend that variable declarations be placed near the top of the file.
It is ok for one line to contain multiple variable declarations:
```
VAR count VAR running_avg
```

If any variables require nonzero initial values these should be set either below the last VAR declaration or inside of an initialization word.


## Capitalization and naming conventions
It is often important to visually distinguish [core words](corewords.md) from [user words](userwords.md).
Therefore we recommend that new code use lower or mixed case for names of user-defined entities.
Underscores are recommended over camelCase to make user words and variables stand out better from core words.

```
var count
: add_to_Count [ .. n -- .. ] count + ! count ;
```

## Stack annotations
As in the above example, we recommend that word definitions include comments to indicate the stack content before after the word is invoked.
The stack annotation should be placed immediately after the word name (on the same line in case the word spans several lines).

The expected prior stack state is written first, followed by a double hyphen (`--`), and then the expected stack state after the word is invoked.

To indicate that it is safe for any amount of other content to exist below the specified content, use a `..` to the left of the indicated stack content.

Stack annotations will often use generic names such as `n`, `m`, `i`, `j`, `x`, `y` to indicate distinct values.
Alternatively, a shorthand name that assigns meaning to stack cells is often helpful:
```
: pulse_pin [.. time pin -- .. ]
  set_pin_On
  wait_time
  set_pin_off ;
```

## Using indentation
As in the above example, we recommend that DSSP code use two-space indentation inside of function definitions.
If the code you are working on uses tabs, set the tabspace to 2 for optimal viewing.


## Multi-line definitions
The previous example illustrates our recommended style for multi-line word definitions, except that a multi-line definition is probably overkill for such a simple word.
Each line should be short enough to understand but it doesn't need to be any specific length.
The semicolon should go on the same line as the last word in the definition.

Using multi-line definitions is also recommended when longer inline comments are used:
```
: main_loop_body
  check_sensors    [This must be first in the loop body]
  IF- sense_error  [If we get bad data, don't do anything!! (invokes EX)]
  plan_action set_outputs ;
```