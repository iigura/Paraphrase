// Project Euler No.56
// usage: ../para PE56.pp

"usage" :
	<<<
		---------------------------
		usage: ../para PE56.pp run
	   \           or
	   \       ../para PE56.pp test
		---------------------------
	>>> write
;

"digit-sum-main" :	// sum target -- sum target
	dup 0 > if
		dup 10 %	// sum target digit
		rot			// target digit sum
		+			// target new-sum
		swap 10 / digit-sum-main
	then
;

// 123 digit-sum -> 6 (==1+2+3)
// I --- I
"digit-sum" : 0 swap digit-sum-main drop ;

"main" :
	[	1 100 for+
			1 100 for+
				i >INT j tuple >pipe
			next
		next
	]

	[[	while-pipe
			clone de-list pow digit-sum
			append >pipe
		repeat
	]]

	{	( max max-pattern ) local-decl
	  	0 `max let
		while-pipe
			@last dup `max , > if
				( max-pattern max ) let
			else
				2drop
			then
		repeat
		`max-pattern , `max ,
	} exec
;

"run" :
	main 
	drop
	( "answer is digitSum(%d^%d)=%d\n" ) 
	swap concat printf
;

"test" :
	main
	972LL != if "NG" .cr -1 exit then
	dup ( 99LL 95 972LL ) != if "NG" .cr -1 exit then
	"GOOD" .cr
;

