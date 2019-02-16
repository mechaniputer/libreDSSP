syn match Comment "\[.*\]"
syn match Print "\.\".*\""
syn keyword basicKeywords NEG ABS BYE IF+ IF0 IF- BR+ BR0 BR- BRS BR
syn keyword basicKeywords IF+ IF0 IF- BR+ BR0 BR- BRS BR
syn keyword basicKeywords BR+ BR0 BR- BRS BR
syn keyword basicKeywords DO D C DS DEEP
syn keyword basicKeywords ET E2 E3 E4
syn keyword basicKeywords CT C2 C3 C4
syn keyword basicKeywords GROW USE SHUT
syn keyword basicKeywords TIN TON SP CR

syn match funDef ": [0-9a-zA-Z]\+"

let b:current_syntax = "dsp"
hi def link Comment       Comment
hi def link Print         Special
hi def link basicKeywords Statement
hi def link funDef        Function
