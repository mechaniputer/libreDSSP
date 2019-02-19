syn case match
syn sync minlines=100
syn iskeyword @,33-64,_,~

syn keyword dsspTodo contained TODO FIXME XXX
syn region dsspComment start="\[" end="\]" keepend contains=dsspTodo,@Spell

syn match dsspNumber '\<[[:digit:]\-]\+\>'

syn region dsspPrint start='\."' end='"'

syn match dsspArithWord '\<\%([+\-*/]\|NEG\|ABS\)\>'
syn match dsspInterpWord '\<\%(\.\|\.\.\|BYE\)\>'
syn match dsspConditionalWord '\<\%(IF[+-0]\|BR[+-0S]\)\>'
syn match dsspLoopingWord '\<DO\>'
syn match dsspStackWord '\<\%(E[234T]\|C[234T]\?\|DS\?\|DEEP\)\>'
syn match dsspIOWord '\<\%(CR\|SP\|T[IO]N\)\>'
syn match dsspDictWord '\<\%(?\$\|GROW\|SHUT\|USE\)\>'
syn match dsspVarWord '\<!\>'

syn match dsspBranch '\<BR\>' nextgroup=dsspLabel skipwhite skipempty
syn match dsspLabel '\<\d\+\>' contained nextgroup=dsspBranchWord skipwhite skipempty
syn match dsspLabel '\<ELSE\>' contained
syn match dsspBranchWord '\<\k\+\>' contained nextgroup=dsspLabel skipwhite skipempty

syn match dsspUnmatchedElse '\<ELSE\>'

syn match dsspDefine ':' nextgroup=dsspFuncName skipwhite skipempty
syn match dsspFuncName '\<\k\+\>' contained nextgroup=dsspFuncBody skipwhite skipempty
syn region dsspFuncBody start='.' matchgroup=dsspFuncTermin end=';' transparent contained

syn match dsspUnmatchedTermin ';'

let b:current_syntax = "dsp"
hi def link dsspComment Comment
hi def link dsspNumber Number
hi def link dsspPrint String
hi def link dsspArithWord Operator
hi def link dsspStackWord Identifier
hi def link dsspIOWord Identifier
hi def link dsspConditionalWord Conditional
hi def link dsspLoopingWord Repeat
hi def link dsspInterpWord Special
hi def link dsspDictWord Special
hi def link dsspVarWord Operator
hi def link dsspBranch Conditional
hi def link dsspLabel Label
hi def link dsspUnmatchedElse Error
hi def link dsspDefine Function
hi def link dsspFuncName Identifier
hi def link dsspFuncTermin Delimiter
hi def link dsspUnmatchedTermin Error
