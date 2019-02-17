[TODO Fix formatting once multiline functions and longer comments/prints work]
: next CR STEP BR 0 step0 1 step1 2 step2 3 step3 4 step4 5 step5 ELSE allDone ;

: incStep STEP 1 + ! STEP ;
: restart DS 0 ! STEP step0 ;

: nextRemind CR ." (Enter 'next' when you are ready to continue)" CR ;

[Printing to the screen]
: step0 ." As a first step, try making the interpreter display a message. You can do" CR ." this by typing a period followed by a double quote, followed by your message," CR ." and finally another double quote before hitting enter. For example you" CR ." could type .'hello' but with double quotes in place of single quotes" CR nextRemind incStep ;

[Placing values, viewing top value]
: step1 ." You can place integers on the stack by typing them and hitting enter." CR ." You can then enter '.' to print the top of the stack.  Try entering" CR ." an integer and then view it with '.'" CR incStep nextRemind ;

[Viewing entire stack with multiple values]
: step2 DEEP BR 0 s2d0 1 s2d1 ELSE s2dd ." Note: you can perform several actions in order" ." by separating them with spaces" CR ." like this: 7 3 ." CR nextRemind ;
: s2d0 ." You must add something to the stack before continuing." CR ;
: s2d1 ." Good. Now add another value to the stack and " ."type '..' to view the whole" CR ." stack at once." CR incStep ;
: s2dd ." Looks like you added more than one value to the stack. " ."Now enter '..' to view" CR ." the whole stack at once." CR incStep ;

[Basic arithmetic]
: step3 DEEP 1 - BR+ s3enough s3few nextRemind ;
: s3few ." Please make sure that there are several values on the stack before continuing." CR ;
: s3enough ." Now we will learn how to perform basic arithmetic on integers using the stack." CR ." The four basic arithmetic operators (+ - * /) all work in a similar way. Entering" CR ." any of these four operators will apply that operator" ." to the top two elements of" CR ." the stack, leaving the result in their place. Give it a try and use '..' to view" CR ." the effect that it has on the stack." CR DEEP ! PREVDEPTH incStep ;

[Clearing the stack]
: step4 PREVDEPTH DEEP - BR- step4bad step4ok nextRemind ;
: step4bad ." It doesn't look like you did any math operations yet." CR ." Please try one of: + - * /" CR ;
: step4ok ." Nice. Now let's completely clear the stack. There are several ways to do this." CR ." One way is to repeatedly enter 'D' to drop the top element until none are left." CR ." another way is to enter 'DS' to drop all elements at once." CR incStep ;

[Defining variables]
: step5 DEEP BR 0 step5empty ELSE step5bad CR ;
: step5empty ." Very good. Unfortunately the rest of the tutorial has yet to be implemented" CR allDone ;
: step5bad ." You need to clear the stack before continuing." CR nextRemind ;

[Defining functions]
[Defining functions with conditionals]
[Defining functions with branches]
[Commenting your code]
[Using recursion]
[Subdictionaries]

: allDone ." Congratulations, you have completed the libreDSSP tutorial!" CR ." As a next step, you may wish to study the source file of this tutorial." CR CR BYE ;

: cheat 5 6 3 DO next + next DS next ;

[We will start at step 0]
0 ! STEP

CR
CR ."           ************************************************"
CR ."           *  Welcome to the libreDSSP tutorial program!  *"
CR ."           ************************************************" CR
." This program will attempt to teach you how to use various libreDSSP" CR
." features. After you complete each step, enter 'next' to continue the" CR
." tutorial. You can restart from the beginning at any time by entering 'restart'" CR CR

next
