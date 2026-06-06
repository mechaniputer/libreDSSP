# Running libreDSSP

## Running Interactively
To launch libreDSSP, enter the directory where the executable is located and run `./dssp` in your shell. You will see the interactive "*" prompt.

## How to exit the libreDSSP interactive prompt

- The libreDSSP prompt normally appears as an asterisk (*) but if there is an incomplete statement it will instead appear as a question mark (?).

- If you see the asterisk prompt, you can type the word `BYE` and press Enter — this cleanly terminates libreDSSP.

- Or press Ctrl-D at the start of a new line (send EOF character).

- If a libreDSSP program is currently running, or if you see the ? prompt, use Ctrl-C.

- **Note: At the moment, pressing Ctrl-C will terminate libreDSSP. This behavior will be changed in a future version to cleanly return to the * prompt.**

## Running a Source File
libreDSSP can open a file from a path specified as a commandline argument. For example:
```
$ ./dssp examples/FIB.DSP
```

If the source file contains words that execute the program, it will run. If it doesn't, then you might have to type something at the prompt to make it run. You should examine the source file for possible instructions in this matter.

If the program itself executes the word `BYE` then libreDSSP will exit upon program completion. Otherwise, you may use the prompt to relaunch the program, or exit using the instructions above.