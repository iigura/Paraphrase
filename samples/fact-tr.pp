"info" :
	"This sample program shows factorial calculation by tail recursion." .cr
	"usage: ../para fact-tr.pp 100LL fact .cr" .cr
;

interactive? not if
	args size 0? if info then
then

"fact-main" : // accum n -- resutl
	dup 0? if
		drop
	else
		dup rot * swap 1- fact-main // n*accum n-1 fact-main
	then
;

"fact" : 1 swap fact-main ;

