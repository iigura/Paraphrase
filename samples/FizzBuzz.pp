// Usage: ../para FizzBuzz.pp 1 20 for+ i FizzBuzz . next cr

windows? if utf8>sjis then

"help" :
	"switch ? dispatch の使い方を示します。" .cr
	"いわゆる FizzBuzz プログラムを実行します。" .cr cr
	"Usage: ../para FizzBuzz.pp 1 20 for+ i FizzBuzz . next cr" .cr cr
;

args size 0? if help then

"FizzBuzz" :
	switch
		case 15 % 0? -> "FizzBuzz"	break
		case  5 % 0? -> "Buzz"		break
		case  3 % 0? -> "Fizz"		break
	dispatch
;

"run" :
	1 20 for+
		i FizzBuzz .
	next
	cr
;

