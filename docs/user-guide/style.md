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


## General code layout
We recommend that variable declarations be placed near the top of the file.
It is ok for one line to contain multiple variable declarations:
```
VAR count VAR running_avg
```

If any variables require nonzero initial values these should be set either below the last VAR declaration or inside of an initialization word.

**Note: libreDSSP does not currently support declaring and initializing a variable on the same line**

Following any variable initializations, define all of the words in the file. For advice about formatting each word definition, keep reading this page.

Lastly, most standalone programs require that words be invoked outside of any definition to begin running the code.
We recommend placing any such code in a single short line at the end of the file (followed by the customary blank line).
If the code that must be run outside of a definition is lengthy (more than three or four words), put it in a new word definition and then invoke that word.


## Capitalization and naming conventions
It is often important to visually distinguish [core words](corewords.md) from [user words](userwords.md).
Therefore we recommend that new code use lower or mixed case for names of user-defined entities.
Underscores are recommended over camelCase to make user words and variables stand out better from core words.

```
VAR count
: add_to_count [ .. n -- .. ]
  count + ! count ;
```

## Stack annotations
As in the above example, we recommend that word definitions include comments to indicate the stack content before and after the word is invoked.
The stack annotation should be placed immediately after the word name (on the same line in case the word spans several lines).

The expected prior stack state is written first, followed by a double hyphen (`--`), and then the expected stack state after the word is invoked.

To indicate that any number of additional cells may exist below the specified content, use a `..` to the left of the indicated stack content.
If a cell drops the stack (for example with DS), be sure to indicate this in the stack annotation:
```
: does_ds [ .. -- ]
  DS ;
```

Stack annotations will often use generic names such as `n`, `m`, `i`, `j`, `x`, `y` to indicate distinct values.
Alternatively, a shorthand name that assigns meaning to stack cells is often helpful:
```
: pulse_pin [ .. time pin -- .. ]
  set_pin_on
  wait_time
  set_pin_off ;
```

To further improve readability, you may want to add stack annotations for certain individual lines that change the stack.
These will usually omit the `--` and only show the stack content following the completion of that line.

As a last note, we point out that some words might be called without knowing the current state of the stack, and will behave differently depending on that state. In such a case we propose two alternatives:

1. You can write a comment explaining what the word does in natural language
2. Experimentally, we propose multi-possibility stack annotations, with each `before -- after` clause separated with a single slash:

```
[ a b c -- a+b+c / a b -- a+b]
```

## Using indentation
As in the above example, we recommend that DSSP code use two-space indentation inside of word definitions.
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

## Formatting the `BR` statement
A BR statement should generally be formatted as a multi-line statement, with one value-target tuple per line, treating the `ELSE` like any other value.
Each tuple should be additionally indented (if already inside of a word definition, this means 4 spaces)

For example, instead of this:
```
: check_val
  BR 0 is_zero 1 is_one 2 is_two ELSE is_other ;
```

Do this:
```
: check_val
  BR
    0 is_zero
    1 is_one
    2 is_two
    ELSE is_other ;
```

## A useful tip about naming branch target words
While not "mandatory", we recommend that helper words that exist solely to perform a small action in the context of a particular `BR` statement be named in a way that associates it with the goal of the `BR` statement (often this will relate to the name of the containing word).

For example, instead of this:
```
: check_temperature
  compare_temp_with_target
  BR
    -1 low
    0 good
    ELSE high ;
```

Do this:
```
: check_temperature
  compare_temp_with_target
  BR
    -1 temperature_low
    0 temperature_good
    ELSE temperature_high ;
```

## Avoiding performance nondeterminism
libreDSSP includes many "convenience" features for developers. While these are intended to be used for interactive development, libreDSSP strives for orthogonality and many of these features can technically be invoked at runtime as well.

The words `GROW`, `VAR`, and `DEL` all perform dynamic allocation behind the scenes, and `DEL` even performs a linear-time search of all user-defined words! Although this should take at most a few milliseconds any production microcontroller, the exact running time of all of these words is dependent on many factors. If memory is low, these words can also fail due to insufficient heap space.

Once implemented, the words `VCTR` and `ARR` will join those above.

Although libreDSSP comes with no warranty, our claim and aspiration is that, if you avoid calling any of the above words after initialization, performance will be consistent and there will be no memory allocations during execution.

**Note** Actually, `TON` also uses malloc, but that's a temporary hack.