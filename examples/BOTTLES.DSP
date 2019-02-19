[ Written by Alan Beadle for libreDSSP  ]
[ https://github.com/beadleha/libreDSSP ]
[ Run with '99 BOTTLES'                 ]

: BOTTLES C 1 + DO BRANCHES ;

: BRANCHES C BR 0 NONE 1 ONE 2 TWO ELSE MANY CR CR ;

: MANY
    C TON ." bottles of beer on the wall, "
    C TON ." bottles of beer!" CR
    ."Take one down and pass it around, "
    1 -
    C TON ." bottles of beer on the wall!"
    ;

: TWO
    C TON ." bottles of beer on the wall, "
    C TON ." bottles of beer!" CR
    ."Take one down and pass it around, "
    1 -
    C TON ." bottle of beer on the wall!"
    ;

: ONE
    C TON ." bottle of beer on the wall, "
    C TON ." bottle of beer!" CR
    ."Take one down and pass it around, "
    1 -
    ."no more bottles of beer on the wall!"
    ;

: NONE
    ."No more bottles of beer on the wall,"
	." no more bottles of beer!" CR
    ."Go to the store and buy some more, "
    99 + TON ." bottles of beer on the wall!"
    ;
