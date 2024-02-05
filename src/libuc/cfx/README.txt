CFX (Convert-From-Xnything)

This directory contains subroutines that convert a string representation
to various integer types (presently: |int|, |long|, and |longlong|).

Some of the subroutines available are:

subroutine	description
--------------------------------------------------------------------------------

cfbin		convert-from-binary
cfoct		convert-from-octal
cfdec		convert-from-decimal
cfxxx		convert-from-anybase (up to base 36 at present)
cfnum		convert-from-number (handles various number formats)
cfdecp		convert-from-decimal-w/precision
cfdect		convert-from-decimal-time (handles time-related suffixes)
cfdecf		convert-from-decimal-floating-point
cfa26		convert-from-alpha-base26 (old, previously used for filenames)
cfgenetic	convert-from-genetic (using genomic bases as digits)
cfchars		convert-from-chars (another "convert-from-any-base")


