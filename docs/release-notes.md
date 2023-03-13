# Release Notes: Changes from version 0.95 to 0.96

## Major changes
* Abolished big-float type
* Organize and redesign inter-thread communication

## Changes to existing Words
* true , false - changed to work correctly in lists
* [[=&gt; - now also supports listings with more elements than the number of cores installed 
* printf - now supports multithreaded
* () - now correctly generates an empty list even in a list
* full-clone - fixed a bug that caused symbols to turn into strings

### Name change
There are no Words that have been renamed.

## Newly added Words
* putc
* word? , @word?
* sleep
* &gt;[ , &gt;&gt;[ , &gt;[[ , &gt;&gt;[[ , &gt;[-&gt; , &gt;&gt;[-&gt; , <br>
  &gt;[[-&gt; , &gt;&gt;[[-&gt; , &gt;[[=&gt; , &gt;[[=&gt; , ]= , ]&gt; , <br>
  ]=&gt; , ]&gt;&gt; , ]=&gt;&gt; , ]]= , ]]&gt; , ]]=&gt; , ]]&gt;&gt; , <br>
  ]]=&gt;&gt;
* join

## Obsolete Words
* &gt;big-float , big-float?
* &gt;child , &gt;parent , child&gt; , parent&gt;

## About Language Processing System 'para'
No changes.

## About the sample programs included in the package
* attr.p8e, which unit tests with attributes, is added.

## Other changes
Nothing in particular.

