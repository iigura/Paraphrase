# Release Notes: Changes from version 0.94 to 0.95

## Major changes
* Behavior of assignments to local variables were changed <br>
    (from full-clone to reference)
* Unnamed Word has been now memory management target
* Debugger supports now multi-threaded
* Symbol names starting with @ are allowed
* Paraphrase program extension changed to .p8e
* Add syntax highlighting file for Vim

## Changes to existing Words
* Symbols are now used as Word's names when defining Word.
* In the switch-dispatch block, it is no longer necessary to write a ‘break'.
* case , -&gt; , -&gt;&gt; , dispatch are now supports 'cond'.
* ‘default’ now equivalent to ‘case true’.
* docol is now an immediate Word.
* The Word ‘i' now works within ‘foreach’ block.
* Several Words now support to symbols:<br>
    alias , def , update , defined? , forget , set-breakpoint , _b 
* ‘dump’ now supports association array and symbol.
* &gt;int , &gt;double now support string.
* &gt;long , &gt;float now support string and address value.
* &gt;symbol supports boolean.
* &gt;word supports symbol and Word.
* Enhanced functionality for multi-level loading.
* Renamed de-list to ‘expand’ and now also supports arrays.
* Unified behavior against to scalar value and each element of container in == and !=.
* The behavior of outputting strings in a list by ‘.’ (dot) has changed.
* Word ‘reduce' and 1+ , 1- , 2/ have been discontinued..
* The following Words were debugged:
    alias , printf , &gt;array , dup , for- , &gt;int , car , @car , cdr , @cdr<br>
	pop-front , &gt;long , docol , expand

### Name change
* Several Words were renamed:<br>
	| from           | to              |
	|----------------|-----------------|
	| &gt;INT        | &gt;big-int     |
    | &gt;FLOAT      | &gt;big-float   |
    | &gt;str        | &gt;string      |
	| &gt;to-hex-str | &gt;hex         |
	| local          | local-decl      |
	| local-with     | local           |
    | type           | @type           |
	| type-name      | @type-name      |
    | empty-str?     | @empty-str?     |
	| not-empty-str? | @not-empty-str? |
    | valid-index?   | @valid-index?   |
    | pop-front      | @pop-front      |
	| add            | @push-back      |
	| append         | @append         |
    | get            | @get            |

## Newly added Words
* :( , );
* (< , >)
* ++ , -- , &lt;- , += , -= , &lowast;= , /= , %=
* +@ , -@ , &lowast;@ , /@ , %@
* &gt;array , &gt;assoc , &gt;bool , &gt;list
* &gt;int? , &gt;long? , &gt;float? , &gt;double?
* @set-attr
* @sqrt
* @type&gt; , &gt;type
* bool-type , int-type , float-type , double-type , word-type
* array? , list? , assoc? , @array? , @assoc? , string?
* int? , long? , big-int? , float? , double? , big-float?
* append , pop-front , push-back
* as-counter
* cond
* docol-target-word
* is?
* last-defined-word
* map-local , map-let
* otherwise-drop , otherwise-2drop
* panic
* replace-str , split-str-by , trim
* reverse-foreach
* round
* self
* set-delete-by-forget , clear-delete-by-forget , check-delete-by-forget
* set-random-seed
* sputf
* this-word-is-removed
* tron?
* when
* zip , min-zip , max-zip , max-zip-with

## About Language Processing System 'para'
* Add show-size option
* Changed exit status (now normal exit even in help and version displays)

## About the sample programs included in the package
* Lisp interpreter LisP8E was added to the sample program.
* A test function has been added to the sample programs.
* PE10mt.p8e, PolishNotation.p8e, fastInvSqrt.p8e, failure.p8e, pass.p8e  were added.

## Other changes
* Added test program driver test.bat for Windows (see 'samples' directory). 
  (on macOS and Linux, you can use 'make test' to run tests).

