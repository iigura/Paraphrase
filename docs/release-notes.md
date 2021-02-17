# Release Notes: Changes from version 0.93 to 0.94

## Major changes
* Added a debugger
* Added pseudo-standard I/O functions
* Support for aspect orientation programming
* Words now have attribute information
* Added associative array
* Added invalid values (open to users)
* Support for cross-references between Words
* Added support for temporary word arguments
* 'here-document' supported

## Changes to existing Words
* Improved consistency of various loop processing
* Specification change for 'switch - dispatch' blocks  
     The Word 'case' is now required.
* Specification change for 'while - repeat' blocks  
     The Word 'do' is now required  
     (this 'do' is a different Word with the same name than the previous 'do').
* Specification change for 'for+' and 'for-' loops  
     The specification has been changed to not include the end value.
* Words with added functionality  
     last, @last - Arrays are now supported.  
     set, get - Added support for associative arrays.  
     exec - now supports Standard Code.  
     map, filter - Added support for the Word i.  
     i - map, filter, find, and reduce are now supported.
* Things that have changed and reappeared in function  
     do

### Name change
* Name change for word prefixes  
     valid? to @valid?  
     'type' to @type
* Name changes due to frequency of word use  
     'local' to local-decl  
     local-with to 'local'

## Newly added Words
* &lt;&lt;&lt;
* &gt;&gt;&gt;
* &gt;&gt;&gt;raw
* &gt;array
* &gt;mock-stdin
* &gt;param
* ...
* +@
* ++
* -@
* --
* &ast;@
* /@
* %@
* &#095;b
* &#095;c
* &#095;cb
* &#095;n
* &#095;s
* @even?
* @has-key?
* @invalid?
* @list?
* @word-name
* 2drop-rs
* add-enter-char
* at
* clear-breakpoint
* cont
* default
* default-docol
* defined?
* docol
* end-with?
* enum
* even?
* find
* get-attr
* get-line-from-mock-stdout
* get-std-code
* has-any-attr?
* has-attr?
* has-key?
* invalid
* invalid?
* max
* min
* new-assoc
* not-true?
* raw&lt;&lt;&lt;
* reduce
* remove-attr
* return
* search
* set-attr
* set-breakpoint
* set-code
* set-worded-docol-caller
* show-attr
* show-trace
* start-with
* step-over
* step-in
* troff
* tron
* update
* use-mock-stdin
* use-mock-stdout
* use-stdin
* use-stdout
* valid?
* word-name
* worded-docol-caller

## About Language Processing System 'para'
No specific changes.

## About the sample programs included in the package
* PE56.pp, a sample program to calculate Project Euler No.56, was added  
     PE56 is a sample program that calculates the largest sum of numbers in each digit when a^b is expressed as a decimal number, where a and b are numbers greater than 1 and less than 100, respectively.
* Change of the file name of the sample program  
     PE21mt-short.pp was renamed to PE21mt.pp.
* Support for unit testing of sample programs  
     Each sample program now supports the argument "test".
