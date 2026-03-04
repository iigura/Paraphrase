# Release Notes: Changes from version 0.97.1 to 0.97.2

## Major changes
There are no major changes in the language system.

## Changes to existing Words
Type-related Words have been updated.
This release involves complex changes,
including renaming existing Words and introducing new ones with the same names.
Please refer to the following section for a detailed explanation of these changes.

### Name change
Previously, the Word used to extract type information from the TOS (Top of Stack) value was `@type>`.
This has been renamed to `@>type`, as it represents a conversion "to type" information.

In conjunction with this change,
a new Word `>type` has been added,
which consumes the TOS value.
Although a Word named `>type` already existed,
it was necessary to rename it to accommodate this restructuring.

Specifically, the Word for performing a cast was previously named `>type`.
However, based on the principle of generating a value "from a type" (casting),
it should logically be named `type>`.
Following this logic, we have renamed the casting Word from `>type` to `type>`.

## Newly added Words
The existing Word `>type` has been renamed,
and a new Word also named `>type` has been added in its place.

## Obsolete Words
No changes.

## About Language Processing System 'para'
There are no significant changes.

## About the sample programs included in the package
The sample program fastInvSqrt.p8e,
which utilizes type information,
has been updated to support the new Word names.

## Other changes
Nothing in particular.

