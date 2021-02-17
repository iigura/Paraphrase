// W --- 
// execute on AOP context.
"myDocol" :
	"IN:myDocol" .cr
	"targetWord=" . word-name .cr

	docol

	"OUT:myDocol" .cr
;

// set myDocol to worded-docol-callee.
"myDocol" >word set-worded-docol-callee

"hello" : "HELLO" .cr ;

// weave myDocol to hello.
"hello" >word worded-docol-caller set-code

// hello

