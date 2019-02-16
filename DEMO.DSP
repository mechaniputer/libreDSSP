[   This file is part of libreDSSP.                                       ]
[                                                                         ]
[   Copyright 2019 Alan Beadle                                            ]
[                                                                         ]
[   libreDSSP is free software: you can redistribute it and/or modify     ]
[   it under the terms of the GNU General Public License as published by  ]
[   the Free Software Foundation, either version 3 of the License, or     ]
[   (at your option) any later version.                                   ]
[                                                                         ]
[   libreDSSP is distributed in the hope that it will be useful,          ]
[   but WITHOUT ANY WARRANTY; without even the implied warranty of        ]
[   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         ]
[   GNU General Public License for more details.                          ]
[                                                                         ]
[   You should have received a copy of the GNU General Public License     ]
[   along with libreDSSP.  If not, see <http://www.gnu.org/licenses/>.    ]


CR CR  ."Hello, world!" CR CR
 

."Addition: 7 + 108 = 115" CR
7 108 + . CR


."Subtraction: 115 - 42 = 73" CR
42 - . CR


."Multiplication: 73 * 4 = 292" CR
4 * . CR


."Division: 292 / 2 = 146" CR
2 / . CR
 

."Stack has stuff: " .. CR
D ."And now it doesn't: " .. CR

."The DO command lets you repeat a word"
SP ."the number of times that is at the top of the stack." CR
."For instance if we push 10 and DO SP then..." 10 DO SP ."we get 10 spaces" CR


CR ."Now let's count from 0 to 10 using BR and recursion!." CR
."BR takes a test value and two branches which"
SP ."must each be a single word, separated by 'ELSE'." CR

: add1 1 + ;
: loop10 add1 C . BR 10 ."Done!" ELSE loop10 ;
0 . loop10

CR CR ."Let's save a variable with: '42 ! meaning'" CR
42 ! meaning
."And now if we clear the stack with 'DS': "
DS ..
."We still know that meaning = " meaning . CR
