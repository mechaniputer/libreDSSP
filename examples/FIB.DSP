[ Written by Alan Beadle for libreDSSP  ]
[ https://github.com/beadleha/libreDSSP ]
[ Compute Fibonacci nums as signed ints ]
[ Stop when a sign overflow occurs      ]
: START 0 . 1 . ;
: GO C2 C2 + CHECK . ;
: CHECK C IF- END ;
: END DS ."Sign overflow" CR EX ;
START RP GO BYE
