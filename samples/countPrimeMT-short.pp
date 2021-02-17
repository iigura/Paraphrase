windows? if utf8>sjis then

"info" : <<<
	----------------------------------
	countPrimeMT-short.pp {run | test}
	----------------------------------
	This program examines the number of primes that exist between 1 and 10 million.
	It uses multi-threads and runs the investigation in parallel. It takes about 19
	seconds on a Mac book air with 4 CPU threads.

	Usage: In the samples directory
    \    ... /para countPrimeMT-short.pp run

	and you should get the following output.
    \    numOfPrimes=664579 (2 ... 9999991)
  >>> write
;

"prime?" :
	switch
		case 1 ==    -> false break
		case 2 ==    -> true  break
		case 2 % 0?  -> false break
		default
			true swap dup sqrt ceil >int 1+ 3 swap
			for+ dup i % 0? if swap drop false swap leave then 2 step next drop
	dispatch
;

interactive? not if
	args size 0? if
      info cr -1 exit
    then 
then

"calc" : 
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
;

"run" :
	calc
	( "numOfPrimes=%d (%d ... %d)\n" )
		over size append
		over car  append
		over last append
	printf
	drop
;

"check" : != if "NG" .cr -1 exit then ;

"test" :
	calc
	@size 664579  check	// num of primes in [1,10^7].
	@car  2       check	// 1st prime
	@last 9999991 check	// last prime in [1,10^7].
	drop
	"GOOD" .cr
;

