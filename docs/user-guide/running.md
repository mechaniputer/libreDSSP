# Running libreDSSP

## How to launch the libreDSSP shell
To launch libreDSSP, enter the directory where the executable is located and run `./dssp` in your shell. You will see the interactive "*" prompt.

## How to exit the libreDSSP shell

- The libreDSSP shell prompt normally appears as an asterisk (*) but if there is an incomplete statement it will instead appear as a question mark (?).

- If you see the asterisk prompt, you can type the word `BYE` and press Enter.

- Or press Ctrl-D at the start of a new line (send EOF character).

- If a libreDSSP program is currently running, or if you see the ? prompt, use Ctrl-C.

- **Note: At the moment, pressing Ctrl-C will terminate libreDSSP. This behavior will be changed in a future version to cleanly return to the * prompt.**

## Evaluating a Source File
libreDSSP can open a file from a path specified as a commandline argument. For example:
```
$ ./dssp examples/FIB.DSP
```

This will evaluate all of the line of code in the source file, and then close DSSP. If you want to keep the shell open after the file has been evaluated, pass the `-i` flag:
```
$ ./dssp examples/FIB.DSP -i
```

If you specify any optional parameters before the filename, you must also pass the -f flag just prior to the filename or you will get an error about the number of parameters:
```
$ ./dssp -i -f examples/FIB.DSP
```
or equivalently,
```
$ ./dssp -if examples/FIB.DSP
```

## Turning off copyright and version information
To prevent printing of copyright and version information, pass the `-q` flag:
```
$ ./dssp -q
*
```

## Evaluating a text string
libreDSSP allows evaluation of a text string passed as a CLI arg:
```
$ ./dssp -qe '."Hello World" CR'
Hello World
$
```

In the example above, the code contained double quotes for [printing a string](io.md). Note that the code string used single quotes to ensure correct behavior. Alternatively, you can use the backslash (\\) character to escape quotes inside the code string.

If both a file and a text string are to be evaluated, the file is always evaluated first, then the text string. Lastly, if `-i` was passed, the interactive shell will open. This sequence allows the text string to be used to select or test specific functionality within the source file.

Only one file and one text string may be passed. This differs from some Forth implementations which allow multiple files and text strings to be evaluated, but is consistent with DSSP's use of structure to avoid confusing or unmaintainable use patterns.

## Pipes and libreDSSP
libreDSSP will behave in the standard useful ways when piping data in or out of DSSP programs. You should probably enable the `-q` flag for the latter.

You can also pipe DSSP code into dssp itself (as long as you use flags that result in starting an interactive shell) but this is not recommended as a standard practice. Just use a file.
