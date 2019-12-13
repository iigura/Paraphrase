windows? if utf8>sjis then

"help" :
	"与えられた数 N に対するメルセンヌ数（2^N-1）が素数か否かを計算します。" .cr
	"usage: ../para Mersenne.pp N prime-test" .cr
	"ex1: M26 = 2^23209-1" .cr
	"     ../para Mersenne.pp 23209 prime-test" .cr
	"ex2: M27 = 2^44497-1" .cr
	"     ../para Mersenne.pp 44497 prime-test" .cr
	"ex3: M28 = 2^86243-1" .cr
	"     ../para Mersenne.pp 86243 prime-test" .cr
;

interactive? not if
	args size 0? if help cr -1 exit then
then

// n --- 2^n-1
"mersenne" : 2 >INT swap pow 1- ;

// n --- t/f
"lucas-lehmer-test" :
	1 over mersenne	// n 1 Mn
	rot	2 - 		// 1 Mn n-2
	rot swap		// Mn 1 n-2
	4 >INT inv-rot		// Mn 4 1 n-2
	for+				// Mn 4
		dup * 2 - over %
	next
	0 ==		 // Mp t/f
	swap drop
;

// fast version of lucas-hehmer-test.
// n --- t/f
"lucas-lehmer-test-fast" :
	dup 1 over mersenne	// n n 1 Mn
	rot	2 - 			// n 1 Mn n-2
	rot swap			// n Mn 1 n-2
	4 >INT inv-rot		// n Mn 4 1 n-2
	for+			// n Mn 4(=Sk)
		dup *		// n Mn sqrt(=Sk*Sk)
		dup 		// n Mn sqrt sqrt
		3 pick		// n Mn sqrt sqrt n
		>>			// n Mn sqrt sqrt>>n
		swap		// n Mn sqrt>>n sqrt
		2 pick		// n Mn sqrt>>n sqrt Mn
		&			// n Mn sqrt>>n sqrt&Mn
		+			// n Mn sqrt>>n+sqrt&Mn(=s)
		dup			// n Mn s s
		2 pick		// n Mn s s Mn
		>= if		// n Mn s
			over -	// n Mn s-Mn
		then
		2 -			// n Mn S_{k+1}
	next
	0 ==			// n Mp t/f
	inv-rot			// t/f n Mp
	drop drop		// t/f
;

// n --- t/f
"lucas-lehmer-test-fast2" :
	dup `p local-with
	1LL swap << 1 - `m local-with
	4LL `s local-with
	2 `p , 1 - for+
		`s , dup * `sqrt local-with
		`sqrt , `m , & `sqrt , `p , >> + `s let
		`s , `m , >= if `s , `m , - `s let then
		`s , 2 - `s let
	next
	`s , 0 ==
;

"prime-test" :
	"just a moment, please..." .cr
	dup lucas-lehmer-test-fast2
	swap "2^%d-1 is " putf
	if "prime" else "not prime" then "%s.\n" putf
;

