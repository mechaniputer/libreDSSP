[ Written by Alan Beadle for libreDSSP  ]
[ https://github.com/beadleha/libreDSSP ]
[ Run with '99 bottles'                 ]

: bottles [n -- n]
  C 1 + [n n+1]
  DO branches ;

: branches [n -- n]
  C BR
  0 no_more
  1 last_beer
  2 two_beers
  ELSE many_beers
  CR CR ;

: take_one_down [n -- n-1]
  ."Take one down and pass it around, "
  1 - ;

: print_beers [n -- n]
  C 2 TON ." bottles of beer on the wall, "
  C 2 TON ." bottles of beer!"
  CR ;

: bobotw [n -- n] ." bottles of beer on the wall!" ;

: many_beers [n -- n] print_beers take_one_down C 2 TON bobotw ;

: two_beers [2 -- 1]
  print_beers
  take_one_down [2 -- 1]
  C 2 TON ." bottle of beer on the wall!" ;

: last_beer [1 -- 0]
  C 2 TON ." bottle of beer on the wall, "
  C 2 TON ." bottle of beer!" CR
  take_one_down [1 -- 0]
  ."no more" bobotw ;

: no_more [0 -- 99]
  ."No more bottles of beer on the wall,"
  ." no more bottles of beer!" CR
  ."Go to the store and buy some more, "
  99 + 2 TON bobotw ;

99 bottles
