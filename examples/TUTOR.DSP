[  This file is part of libreDSSP.                                       ]
[                                                                        ]
[  Copyright 2019 Alan Beadle                                            ]
[                                                                        ]
[  libreDSSP is free software: you can redistribute it and/or modify     ]
[  it under the terms of the GNU General Public License as published by  ]
[  the Free Software Foundation, either version 3 of the License, or     ]
[  (at your option) any later version.                                   ]
[                                                                        ]
[  libreDSSP is distributed in the hope that it will be useful,          ]
[  but WITHOUT ANY WARRANTY; without even the implied warranty of        ]
[  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         ]
[  GNU General Public License for more details.                          ]
[                                                                        ]
[  You should have received a copy of the GNU General Public License     ]
[  along with libreDSSP.  If not, see <http://www.gnu.org/licenses/>.    ]

[Interactive libreDSSP tutor]

: next CR STEP BR 
	0 step0
	1 step1
	2 step2
	3 step3
	4 step4
	5 step5
	6 step6
	7 step7
	8 step8
	9 step9
	ELSE allDone ;

: incStep STEP 1 + ! STEP ;
: restart DS 0 ! STEP step0 ;
: cheat 5 6 3 DO next + next DS next CR step6good 3 DO next ;

: nextRemind CR ." (Enter 'next' when you are ready to continue)" CR ;

[Printing to the screen]
: step0 ." As a first step, try making the interpreter display a message. You can do" CR
	." this by typing a period followed by a double quote, followed by your message," CR
	." and finally another double quote before hitting enter. For example you" CR
	." could type .'hello' but with double quotes in place of single quotes" CR
	incStep nextRemind ;

[Placing values, viewing top value]
: step1 ." You can place integers on the stack by typing them and hitting enter." CR
	." You can then enter '.' to print the top of the stack.  Try entering" CR
	." an integer and then view it with '.'" CR
	incStep nextRemind ;

[Viewing entire stack with multiple values]
: step2 DEEP BR 0 s2d0 1 s2d1 ELSE s2dd
	." Note: you can perform several actions in order by separating them with spaces" CR
	." like this: 7 3 ." CR
	nextRemind ;
: s2d0 ." You must add something to the stack before continuing." CR ;
: s2d1 ." Good. Now add another value to the stack and type '..' to view the whole" CR
	." stack at once." CR
	incStep ;
: s2dd ." Looks like you added more than one value to the stack. Now enter '..' to view" CR
	." the whole stack at once." CR
	incStep ;

[Basic arithmetic]
: step3 DEEP 1 - BR+ s3enough s3few nextRemind ;
: s3few ." Please make sure that there are several values on the stack before continuing." CR ;
: s3enough ." Now we will learn how to perform basic arithmetic on integers using the stack." CR
	." The four basic arithmetic operators (+ - * /) all work in a similar way. Entering" CR
	." any of these four operators will apply that operator" ." to the top two elements of" CR
	." the stack, leaving the result in their place. Give it a try and use '..' to view" CR
	." the effect that it has on the stack." CR
	DEEP ! PREVDEPTH incStep ;

[Clearing the stack]
: step4 PREVDEPTH DEEP - BR- step4bad step4ok nextRemind ;
: step4bad ." It doesn't look like you did any math operations yet." CR
	." Please try one of: + - * /" CR ;
: step4ok ." Nice. Now let's completely clear the stack. There are several ways to do this." CR
	." One way is to repeatedly enter 'D' to drop the top element until none are left." CR
	." Another way is to enter 'DS' to drop all elements at once." CR
	incStep ;

[Defining variables]
: step5 DEEP BR 0 step5empty ELSE step5bad nextRemind ;
: step5empty ." Very good. Now you will learn to define a function." CR
	." A function declaration begins with a ':' followed by the name of the function, followed by" CR
	." the function body, ending with a ';'. Each of these are space-separated. For example to" CR
	." define a function that adds 2 to the value on top of the stack and prints the result, you could enter:" CR
	." : addtwo 2 + . ;" CR CR
	." To proceed, define a function named double which multiplies the value at the top of the" CR
	." stack by two. You can optionally use the 'C' operator which makes a clone of the item at the top" CR
	." of the stack. You could also use multiplication." CR
	incStep ;
: step5bad ." You need to clear the stack before continuing." CR ;

[Defining functions]
: double ; [The user must redefine this]
: step6 42 double BR 84 step6b ELSE step6bad nextRemind ;
: step6b -4 double BR -8 step6good ELSE step6bad ;
: step6bad ." Uh oh. Your double function isn't working. Try one of the solutions below:" CR
	." : double C + ;" CR
	." : double 2 * ;" CR ;
: step6good ." Nice! Looks like you were able to successfully define the 'double' function!" CR
	." Here are two valid solutions:" CR
	." : double C + ;" CR
	." : double 2 * ;" CR
	incStep ;

[Using IF conditionals]
: step7 ." Next you will learn to use conditionals. libreDSSP has many conditional operators including:" CR
	." IF- IF0 IF+ BR+ BR0 BR- BRS BR" CR
	." First we will learn about the three IF operators. These operators must be followed by at least one" CR
	." word. Each type of IF statement has a different condition. If the condition is met then the word following" CR
	." the IF statement is evaluated normally. If the condition is not met then the next word is skipped." CR
	." The conditions for each of the IF statements are straightforward. IF+ evaluates the next word if the top" CR
	." of the stack is a positive number and otherwise skips it. IF- does the opposite, evaluating the following" CR
	." word only when the top of the stack is negative. IF0 evaluates the following word if the top of the stack" CR
	." is 0 and skips it if the top of the stack is nonzero." CR
	." Try putting numbers on the stack and typing things like: IF+ .'positive' (but use double quotes to make" CR
	." the printing work)" CR
	incStep
	nextRemind ;

[Using BR+ BR0 BR- BRS]
: step8 ." This step will teach you about some of the BR conditionals. Unfortunately it hasn't been implemented yet." CR
	incStep
	nextRemind ;

[Using BR ELSE]
: step9 ." This step will teach you about the BR-ELSE conditional. Unfortunately this step hasn't been implemented yet." CR
	allDone
	incStep
	nextRemind ;

[Defining functions with conditionals]
[Commenting your code]
[Using recursion]
[Subdictionaries]

: allDone ." Congratulations, you have completed the libreDSSP tutorial!" CR
	." As a next step, you may wish to study the source file of this tutorial." CR CR
	BYE ;

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
