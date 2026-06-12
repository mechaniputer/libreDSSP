[ Written by Alan Beadle for libreDSSP  ]
[ https://github.com/beadleha/libreDSSP ]
[ Compute Fibonacci nums as signed ints ]
[ Stop when a sign overflow occurs      ]

: start 0 . 1 . RP run ;
: run C2 C2 + check . ;
: check C IF- end ;
: end DS ."Sign overflow" CR EX ;
start
