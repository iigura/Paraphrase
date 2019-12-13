"help" : 
	cr
	"This is a sample program of for+ ~ next loop." .cr
	"This program outputs a Japanese multiplication table." .cr
	cr
	"usage: ../para 99.pp run" .cr
	cr
;

"run" :
	1 9 for+
		i
		1 9 for+ dup i * "%2d " putf next
		drop cr
	next
;

args size 0? if help then 

