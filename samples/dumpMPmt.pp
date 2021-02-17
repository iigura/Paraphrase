// usage: para dumpMPmt.pp

"help" : <<<

	Searches for and displays the Mersenne primes up to the 20th number.

	\tusage: ../para dumpMPmt.pp {run | test}

	>>> write
;

interactive? not if
	args size 0? if help then
then

"mersenne" : 2LL swap pow 1- ;

"lucas-lehmer-test-fast-by-stack" :
	dup 1 over mersenne	rot	2 - rot swap 4LL inv-rot 1+
	for+
		dup * dup 3 pick >>	swap 2 pick	& +	dup	2 pick	
		>= if over - then
		2 -	
	next
	0 == inv-rot drop drop
;

// p lucas-lehmer-test-fast
"lucas-lehmer-test-fast" :
	`p local
	4LL `s local
	2LL `p , pow 1- `M local
	`sqrt local-decl
	2LL `p , for+
		`s , dup * `sqrt let
		`sqrt , `M , & `sqrt , `p , >> + `s let
		`s , `M , >= if `s , `M , - `s let then
		`s , 2 - `s let
	next
	`s , 0 ==
;

"prime?" :
	switch
		case 1 ==	-> false break
		case 2 ==	-> true  break
		case 2 % 0?	-> false break 
		default
	    	true swap dup sqrt ceil >int 3 swap 1+
			for+ dup i % 0? if swap drop false swap leave then 2 step next
    		drop
	dispatch
;

"num-of-digit" :
	dup log10 floor >int 1+ dup 4 +
	for+ dup 10 >INT i >int pow < if drop i 1- leave then next
	1+
;

// n ---
"calc-and-print-mersenne" :
	dup "p=%5d, " putf
	mersenne dup num-of-digit dup 13 > if
		dup inv-rot
		5 - 10 >INT swap pow swap dup rot / write	
		"..." write 10 5 pow % "%05d" putf " (%5d digits)" putf
	else
		drop "%13d" putf
	then
;

"calc" :
	// check 1...4 by single-thread
	() 1 5 for+
		i prime? if
			i mersenne prime? if i append then
		then
	next

	// check 4...5000 by multi-thread
	reset-pipes
	[ 5 5001 for+ i >pipe 2 step next ]
	[[
		while-pipe
			dup prime? if
				dup lucas-lehmer-test-fast if >pipe then
			then
		repeat
	]]
	while-pipe append repeat { < } sort
;

"run" :
	calc
	0 >r {
		r> 1+ dup "No=%2d, " putf >r
		calc-and-print-mersenne cr
	} foreach
;

"check" : != if "NG" .cr -1 exit then ;

"test" :
	calc
	last 4423 check
	4423 mersenne 
	dup num-of-digit 1332 check
	dup 10LL 1332 5 - pow / 28554 check
	100000 % 80607 check
	"GOOD" .cr
;

