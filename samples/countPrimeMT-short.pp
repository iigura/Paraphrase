// 1〜1000 万までに存在する素数の個数を調べる。

"prime?" :
	switch
		case 1 ==    -> false break
		case 2 ==    -> true  break
		case 2 % 0?  -> false break
		// otherwise
		true swap dup sqrt ceil >int 3 swap
		for+ dup i % 0? if swap drop false swap leave then 2 step next drop
	dispatch
;

reset-pipes
// 奇数については並列処理で調べる
[ 3 10000000 for+ i >pipe 2 step next ]

[[	// 複数スレッドで検証作業（=woker thrread)
	while-pipe
		dup prime? if >pipe else drop then
	repeat
]]

() 2 dup prime? if append else drop then // 2 については、ここで調査
while-pipe append repeat { < } sort
( "numOfPrimes=%d (%d ... %d)\n" ${ size } ${ car } ${ last } ) printf
drop

