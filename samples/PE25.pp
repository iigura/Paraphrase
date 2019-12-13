// Usage: para PE25.pp 1000 PE25 cr

// Project Euler No.25 -> 1000 PE25
// n ---    10^n を超える最初のフィボナッチ数列 ( 1 1 2 3 ... ) の項数を求める
"PE25" :
	1- 10 >INT swap pow
	2 1 1 >INT true while
		swap over + rot 1+ inv-rot dup 4 pick <
	repeat
	drop drop . drop
;

// count F_{n-1} Fn -{ swap over +  rot 1+ inv-rot }- count+1 Fn F_{n+1}
/*
	note:
		count F_{n-1} Fn に対し、swap over +  rot 1+ inv-rot とすると、
		count+1 Fn F_{n+1} を得る。
*/

