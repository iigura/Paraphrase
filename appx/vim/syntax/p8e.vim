" Vim syntax file
" Language: Paraphrase
" Maintainer:    Koji Iigura (Twitter @paraphrase_lang)
" Last Change:   2021 Oct 30

" Quit when a (custom) syntax file was already loaded
if exists("b:current_syntax")
  finish
endif

syn case match

set iskeyword+=-,+,>,!,63	" 63 is character code of '?'
set iskeyword+=/,42
set iskeyword+=@-@		" '@'

syn match   p8eSymbol      /`\S\+/
syn match   p8eComment     /\/\/\_s.*/
syn match   p8eComment     /\/\*\_s.*\_s\*\// 

syn match   p8eNumber      /\<[+-]\{-,1}\d\+\(LL\|ll\|L\|l\)\{-,1}\>/
syn match   p8eNumber      /\<[+-]\{-,1}0[Xx]\x\+\(LL\|ll\|L\|l\)\{-,1}\>/
syn match   p8eNumber      /\<[+-]\{-,1}\d\+\.\d\+\([Ee][+-]\{-,1}\d\+\)\{-,1}[df]\{-,1}\>/

syn region  p8eString matchgroup=p8eQuotes start=+"+ skip=+\\\\\|\\\"+ end=+"+
syn region  p8eString matchgroup=p8eQuotes start=+'+ skip=+\\\\\|\\\'+ end=+'+
syn match   p8eString      /\(raw\)\?<<<\_s\_.*\_s>>>\(raw\)\?/ 

" starndard words
syn keyword p8eRepeat       while loop while-pipe repeat for+ for- next do when
syn keyword p8eStatement    leave continue redo	return
syn keyword p8eKeyword      step i j
syn keyword p8eConditional  if else then switch dispatch
syn keyword p8eLabel        case default ->> ->
syn keyword p8eBoolean      true false and or xor not not-true?

syn keyword p8eMath         sqrt deg-to-rad rad-to-deg set-random-seed rand-to
syn keyword p8eMath         even? @even?
syn keyword p8eMath         square sqrt exp pow log log10 abs sin cos tan round
syn keyword p8eMath         asin acos atan randomize rand random floor ceil min max

syn keyword p8eStdType      >int >long >big-int >float >double >big-float >bool
syn keyword p8eStdType      >string >hex >symbol >array >list >assoc >word >address
syn keyword p8eStdType      >int? >long? >float? >double?
syn keyword p8eStdType      int? long? big-int? float? double? big-float?
syn keyword p8eStdType      bool? string? array? list? assoc? invalid? valid?
syn keyword p8eStdType      @array? @list? @assoc? @invalid? @valid?
syn keyword p8eStdType      type-name @type-name @type> >type is?

syn keyword p8eStringWord   @empty-str? @not-empty-str?
syn keyword p8eStringWord   @search search
syn keyword p8eStringWord   start-with? @start-with? end-with? @end-with?
syn keyword p8eStringWord   replace-str replace >upper >lower trim
syn keyword p8eStringWord   split-str-by
syn keyword p8eStringWord   make-literal-code-list str-to-list-byte
syn keyword p8eStringWord   utf8>sjis sjis>utf8 thru ccc-str
syn keyword p8eStringWord   eval

syn keyword p8eStdWord      full-clone clone

syn keyword p8eStack        clear clear-rs swap swap-rs drop drop-rs 2drop 2drop-rs
syn keyword p8eStack        inv-rot
syn keyword p8eStack        >r @>r r> @r>
syn keyword p8eStack        empty? depth dup 2dup over pick 3drop rot

syn keyword p8eStdWord      size @size at set @set get @get @valid-index? last @last

syn keyword p8eDefWord      defined? set-delete-by-forget clear-delete-by-forget
syn keyword p8eDefWord      check-delete-by-forget get-voc
syn keyword p8eDefWord      const const! >lit >here last-defined-word
syn keyword p8eDefWord      def update forget enum alias

syn keyword p8eConst        pi rand-max
syn keyword p8eConst        bool-type int-type long-type float-type double-type
syn keyword p8eConst        word-type

syn keyword p8eArray		new-array new-array-with 

syn keyword p8eList         empty-list? not-empty-list? @empty-list? @not-empty-list?
syn keyword p8eList         en-list expand @expand append @append push-back @push-back
syn keyword p8eList         tuple car @car cdr @cdr pop-front @pop-front last @last
syn keyword p8eList         split concat slice product zip min-zip max-zip max-zip-with
syn keyword p8eList         map vmap sort sort+ sort- filter find has? @has? in? @in?
syn keyword p8eList         foreach reverse-foreach


hi link p8eRepeat      Repeat
hi link p8eStatement   Statement
hi link p8eKeyword     Keyword
hi link p8eConditional Conditional
hi link p8eLabel       Label
hi link p8eBoolean     Boolean

hi link p8eSymbol      Special
hi link p8eComment     Comment

hi link p8eNumber      Number
hi link p8eString      String

hi link p8eMath        Keyword
hi link p8eDefWord     Function
hi link p8eStringWord  Function 
hi link p8eStack       Function
hi link p8eArray       Function
hi link p8eList        Function
hi link p8eStdWord     Macro
hi link p8eConst       Macro
hi link p8eStdType     Type

let b:current_syntax = "paraphrase"

