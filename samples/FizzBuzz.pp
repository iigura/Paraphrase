"FizzBuzz" :
	switch
		case 15 % 0? -> "FizzBuzz" break
		case 5 % 0?  -> "Buzz"	  break
		case 3 % 0?  -> "Fizz"	  break
		dup 
	dispatch
	swap drop
;

