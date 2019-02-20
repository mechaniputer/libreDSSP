[ Written by Alan Beadle for libreDSSP  ]
[ https://github.com/beadleha/libreDSSP ]
[ Run with '99 BOTTLES'                 ]

: BOTTLES C 1 + DO BRANCHES ;
: BRANCHES C BR 0 NONE 1 ONE 2 TWO ELSE MANY CR CR ;
: TAKE ."Take one down and pass it around, " 1 - ;
: HOWMANY C 2 TON ." bottles of beer on the wall, " C 2 TON ." bottles of beer!" CR ;
: BOBOTW ." bottles of beer on the wall!" ;
: MANY HOWMANY TAKE C 2 TON BOBOTW ;
: TWO HOWMANY TAKE C 2 TON ." bottle of beer on the wall!" ;
: ONE C 2 TON ." bottle of beer on the wall, "
    C 2 TON ." bottle of beer!" CR TAKE ."no more" BOBOTW ;
: NONE ."No more bottles of beer on the wall,"
	." no more bottles of beer!" CR
    ."Go to the store and buy some more, "
    99 + 2 TON BOBOTW ;
