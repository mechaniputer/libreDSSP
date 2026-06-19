# User-defined Words

## Defining words
To define a new word, enter a statement in the following format:

```
: word_name body1 body2 [body 3 ...] ;
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

## Redefining words
If you need to change the definition of the word, you may simply define it again with a different body. No special steps are needed. The old definition will be seamlessly replaced and any word that invokes the updated word will already use the new version.

If you want to rebind the name of a word to a variable, you must use `DEL` to delete the word first.
The word `DEL` will only delete words from the [subdictionary](dictionaries.md) currently selected for modification.

## Undefined words
As mentioned on the [DSSP Jargon](jargon.md) page, DSSP allows you to reference (but not use) undefined words.
For example, consider the following code:
```
* : foo 1 2 3 .. add_3 ;
```
This code will be compiled without error, even if the `add_3` word is currently undefined.
If you run `foo`, it will push the three numbers to [the stack](datastack.md), print the stack (`..`), and then display an error when it finds no definition for `add_3`.
```
* foo
[1 2 3]
ERR: Undefined word add_3 called during execution
[Execution aborted]
*
```

If you subsequently define `add_3`, the old definition of `foo` will automatically use that definition when you try again:
```
* : add_3 + + .. ;
* DS
* foo
[1 2 3]
[6]
*
```

As a consequence of these features, note that if you write part of a program and test it, as long as you do not hit any of the undefined words during the test, *(or until you do)* you will be able to examine how the finished program will behave for the inputs you provide.

## Displaying currently defined words
The above might make you think, *"What if I forget to define a word and deploy code that will fail when that word is called?"* Well first of all, don't do that. But also, you can print a list of all referenced but undefined words by executing the core word `UNDEF`:
```
* : foo bar baz ;
* UNDEF
baz
bar
```

## A note about subdictionaries
When you define a word, it will go into whatever subdictionary is selected for expansion. By default, this will be a subdictionary called `$DEFAULT`.
Words from one subdictionary can reference and use words from any other subdictionary, but this can also be restricted.
For more about the use of subdictionaries in libreDSSP, refer to the [relevant page](dictionaries.md).
