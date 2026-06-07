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

Note the use of spaces. There is a mandatory space following ':', and a mandatory space following each word used in the word body, including up to the closing ';'.

It is permissible to split a word defition across multiple lines:

```
: square
    C 2 * ;
```

## Redefining
If you need to change the definition of the word, you may simply define it again with a different body. No special steps are needed. The old definition will be seamlessly replaced and any word that invokes the updated word will already use the new version.

## Undefined words

## Displaying currently defined words

## A note about dictionaries