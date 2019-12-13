@more +1 %~f0 | ..\para -q %* & exit /b %ERRORLEVEL%
// Paraphrase program in .bat file.
// pseudo "#!../para" on Linux or MacOS.

"=== This is pseudo shebang file. ===" .cr
"This FizzBuzz program is written in a batch file." .cr

"FizzBuzz" :
	switch
		case 15 % 0? -> "FizzBuzz"	break
		case  5 % 0? -> "Buzz"		break
		case  3 % 0? -> "Fizz"		break
	dispatch
;

1 20 for+ i FizzBuzz . next cr

