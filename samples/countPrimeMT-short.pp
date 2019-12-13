windows? if utf8>sjis then

"info" :
	"このプログラムでは 1〜1,000 万までに存在する素数の個数を調べます。" .cr
	"マルチスレッドを使用し、並列にて調査を行います。" .cr
	"4 CPU thread の mac book air で 15 秒程度かかります。" .cr
	"使い方は、samples ディレクトリにて" .cr
	"    ../para countPrimeMT-short.pp run" .cr
	"として下さい。" .cr
	"以下のような出力が得られるはずです:" .cr
	"    numOfPrimes=664579 (2 ... 9999991)" .cr
;

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

interactive? not if
	args size 0? if
      info cr -1 exit
    then 
then

"run" : 
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
	( "numOfPrimes=%d (%d ... %d)\n" )
		over size append
		over car  append
		over last append
	printf
	drop
;

