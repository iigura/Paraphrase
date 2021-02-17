"info" : <<<
		usage: ../para zdk.pp run
	>>> write
;

interactive? not if
	args size 0? if info then
then

"zdk" :
	0 `zun-counter local
	while true do
		random 0.5 > if
			"Zun" .
			`zun-counter ++
		else
			"Doko" .
			`zun-counter , 4 >= if
				"Ki-Yo-Shi!" .
				leave
			else
				0 `zun-counter let
			then
		then
	repeat
;

"run" : zdk ;

"check" : not if "NG" .cr -1 exit then ;

"test" :
	use-mock-stdout zdk use-stdout
	get-line-from-mock-stdout >array 

	@size `n local
	`n , 6 > check
	@last "Ki-Yo-Shi!" == check
	`n , 2 - get "Doko" == check

	0 `counter local
	invalid `endPos local
	0 `n , for+
		i get "Zun" == if
			`counter ++
		else
			`counter , 4 >= if
				i `endPos let
				leave
			then
			0 `counter let
		then
	next
	`endPos , `n , 2 - == check

	"GOOD" .cr
;

