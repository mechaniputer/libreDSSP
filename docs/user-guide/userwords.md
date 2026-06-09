# User-defined Words

## Defining
To define a new word, enter a statement in the following format:

```
: [Word name] [Body] ;
```

For example, the following statement defines a function named square that squares a number:

```
: square C 2 * ;
```

Note the use of spaces. There is a mandatory space following `:`, and a mandatory space following each word used in the word body, including up to the closing `;`.

It is permissible to split a word definition across multiple lines:

```
: square
    C 2 * ;
```

## Redefining
If you need to change the definition of the word, you may simply define it again with a different body. No special steps are needed. The old definition will be seamlessly replaced and any word that invokes the updated word will already use the new version.

## Undefined words
As mentioned on the [DSSP Jargon](jargon.md) page, DSSP allows you to reference (but not use) undefined words.
For example, consider the following code:
```
* : FOO 1 2 3 .. ADD3 ;
```
This code will be compiled without error, even if the `ADD3` word is currently undefined.
If you run `FOO`, it will push the three numbers to [the stack](stack.md), print the stack (`..`), and then display an error when it finds no definition for `ADD3`.
```
* FOO
[1 2 3]
ERR: Undefined word ADD3 called during execution
[Execution aborted]
*
```

If you subsequently define `ADD3`, the old definition of `FOO` will automatically use that definition when you try again:
```
* : ADD3 + + .. ;
* DS
* FOO
[1 2 3]
[6]
*
```

As a consequence of these features, note that if you write part of a program and test it, as long as you do not hit any of the undefined words during the test, *(or until you do)* you will be able to examine how the finished program will behave for the inputs you provide.

## Displaying currently defined words
The above might make you think, *"What if I forget to define a word and deploy code that will fail when that word is called?"* Well first of all, don't do that. But also, you can print a list of all referenced but undefined words by executing the core word `UNDEF`:
```
* : FOO BAR BAZ ;
* UNDEF
BAZ
BAR
```

## A note about dictionaries
When you define a word, it will go into whatever dictionary is selected for expansion. By default, this will be a dictionary called `$DEFAULT`.
Words from one dictionary can reference and use words from any other dictionary.
For more about the use of dictionaries in libreDSSP, refer to the [relevant page](dictionaries.md).
