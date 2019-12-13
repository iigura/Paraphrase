"usage" :
	"../para PE21mt-short.pp 10000 run" .cr
;

interactive? not if
	args size 0? if usage then
then

"d-func" : // n --- d-func-value  see https://projecteuler.net/problem=21
	1 swap  dup 2 /  2 swap  	// result n 2 n/2
	for+	// result n
		dup i % 0 == if	swap  i +  swap then
	next
	drop
;

"make-d-array/mt" :
	reset-pipes
	dup 1 +	 new-array swap		/* array n	*/
	>pipe		/* send n to child */
				/* array	*/
	[	pipe>  2 swap  for+ i >pipe next ]
	[[	while-pipe dup d-func tuple >pipe repeat ]]

	while-pipe	/* array (t d-value-of-t) */
		de-list	set
	repeat
	1 -1 set	/* d(1)=-1 */
;

/* (array n -- array t/f) */
"is-not-perfect-number/array" :
	dup  rot rot  get rot != 
;

/* (array from to -- ) */
"getAmicableNumbersFromArray/mt" :
	reset-pipes
	tuple	/* array ( from to ) */

	/* init DS by the TOS (list:( from to )). */
	[-> de-list /* from to */    for+ i >pipe next ]

	/* --- worker --- */
	/* init DS by the TOS (array). */
	[[-> while-pipe	/* t:target */
			@>r is-not-perfect-number/array if
				@r> get	 valid-index? if 
					get	 /* array d(d(t)) */
					@r> == if r> >pipe else drop-rs then
				else				/* array d(t) */
					drop drop-rs	/* array */
				then				
			else
				drop-rs
			then
		repeat
	]]

	/* --- gather (or reduce) --- */
	[ () while-pipe append repeat >pipe ] 
	pipe>
;

"printAmicableNumbers/mt" :
	dup make-d-array/mt	 swap 2 swap	/* array 2 n */
	getAmicableNumbersFromArray/mt { < } sort
	dup . cr
	0 swap while{ @not-empty-list? }-> pop-front rot + swap repeat
	drop . cr
;

"run" "printAmicableNumbers/mt" alias

