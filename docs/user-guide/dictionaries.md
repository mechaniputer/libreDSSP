# DSSP Dictionaries
DSSP uses a structure called a dictionary to track word definitions and variable declarations.

The dictionary consists of various named subdictionaries.
In libreDSSP, a subdictionary name must begin with a `$` symbol, to distinguish them from other kinds of named entities.
There exists a subdictionary named `$PRIME` containing all of the [core words](corewords.md). `$PRIME` cannot be modified or affected by programs.

On launch, libreDSSP also created a `$DEFAULT` subdictionary for user-defined entities including [words](userwords.md) and [variables](vars.md).


## Disclaimer
Several key features of the DSSP dictionary system are currently unimplemented in libreDSSP.
The features that exist are likely to have bugs, and current edge cases may be subject to change.
If you discover any problems relating to these features, please don't hesitate to open an issue on the [GitHub repo](https://github.com/mechaniputer/libreDSSP).


## Viewing a list of subdictionaries
To see a list of all subdictionaries and their respective status, use `?$`:
```
* ?$
$DEFAULT OPEN
$PRIME OPEN
*
```

The subdictionary at the top of the list is the one currently selected for growth.
The status of a subdictionary is either `OPEN` or `CLOSED`.
An `OPEN` subdictionary can be used when looking for previously defined entities, but a `CLOSED` one cannot.
If the dictionary currently selected for growth is `CLOSED`, new entities cannot be added.


## Opening a new subdictionary
To open a new subdictionary which did not previously exist, use `GROW`.
The new subdictionary will be automatically selected for growth so that subsequently defined entities will go into it.

```
* GROW $MATH
* ?$
$MATH OPEN
$PRIME OPEN
$DEFAULT OPEN
*
```

## Choosing subdictionaries for growth
The `GROW` word used above to create a new subdictionary is also used to re-select a previously defined subdictionary for growth.
```
* GROW $DEFAULT
* ?$
$DEFAULT OPEN
$PRIME OPEN
$MATH OPEN
*
```

## Shutting subdictionaries
By shutting a subdictionary, you ensure that any new word references will ignore the contents of that subdictionary.
```
* : HELLO ."Hello from $DEFAULT" CR ;
* SHUT $DEFAULT
* HELLO
ERR: Symbol HELLO is not an executable word
```

This is useful if there is a risk or expectation of name collisions across multiple subdictionaries.
```
* : FOO ."Foo the first" CR ;
* GROW $NEW
* SHUT $DEFAULT
* : FOO ."Foo the second" CR ;
* FOO
Foo the second
* ?$
$NEW OPEN
$PRIME OPEN
$DEFAULT CLOSED
*
* USE $DEFAULT
* FOO
Foo the second
* SHUT $NEW
* FOO
Foo the first
*
```

## Reopening a closed subdictionary
The word `USE` sets a subdictionary status to `OPEN`.

```
* ?$
$NEW OPEN
$PRIME OPEN
$DEFAULT CLOSED
* USE $DEFAULT
* ?$
$NEW OPEN
$PRIME OPEN
$DEFAULT OPEN
```

However, `USE` does not change the search order. Dictionaries near the top of the list will be searched for matching words first.
To force the use of a word defined in a lower subdictionary you may have to close other subdictionaries:
```
* FOO
Foo the second
* SHUT $NEW
* FOO
Foo the first
*
```

## Some details to be aware of
- Shutting a subdictionary does not prevent prior references to words therein from working. A program will run as usual with all subdictionaries closed as long as no new entities are declared or defined.
- Although reopening a subdictionary might make new words available for searching, it will not trigger resolution of undefined words. A future version of libreDSSP might introduce a word to force a search for new definitions.
- Currently there is no good way to modify the dictionary search order. You might have to `SHUT` one subdictionary to use a word instance from another. A future version will add words to manage subdictionary search order.
