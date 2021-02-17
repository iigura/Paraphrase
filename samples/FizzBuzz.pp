// Usage: ../para FizzBuzz.pp 1 20 for+ i FizzBuzz . next cr

windows? if utf8>sjis then

"help" : <<<
	----------
	This sample program shows how to use the "switch ~ dispatch" block.
	In this sample, we will run the so-called FizzBuzz program.

	You can use it as:

	\     ../para FizzBuzz.pp 1 20 for+ i FizzBuzz . next cr

	or as the same content can also be obtained by giving the argument "run".

	\     ../para FizzBuzz.pp  run
	----------
	>>> write
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

"test" :
	use-mock-stdout
		run
	use-stdout

	get-line-from-mock-stdout
	" 1 2 Fizz 4 Buzz Fizz 7 8 Fizz Buzz 11 Fizz 13 14 FizzBuzz 16 17 Fizz 19"
	!= if "NG" .cr -1 exit then
	"GOOD" .cr
;

