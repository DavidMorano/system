STRXCMP

This directory contains miscellaneous c-string comparison operations.

subroutines	description
--------------------------------------------------------------------------------

strbasecmp{x}	compare c-strings case sensitivity
strcasecmp{x}	compare c-strings using upper case (semi-insensitivity)
strfoldcmp{x}	compare c-strings using folding case (insensitivity)

strkeybasecmp{x}	compare c-strings case sensitivity
strkeycasecmp{x}	compare c-strings using upper case (semi-insensitivity)
strkeyfoldcmp{x}	compare c-strings using folding case (insensitivity)

stremacmp	compare c-strings that are E-Mail-Addresses
strkeycmp	compare c-strings that are key=value pairs (cases available)
strkeydictcmp	compare c-strings w/ dictionary-order
strleadcmp	compare c-strings useing a leading string algorithm
strpcmp		compare c-strings using a prefix algorithm
strvalcmp	compare c-strings that are the values of key-value pairs

strxbasecmp{x}	compare c-strings case sensitivity
strxcasecmp{x}	compare c-strings using upper case (semi-insensitivity)
strxfoldcmp{x}	compare c-strings using folding case (insensitivity)

strkeyxbasecmp{x}	compare c-strings case sensitivity
strkeyxcasecmp{x}	compare c-strings using upper case (semi-insensitivity)
strkeyxfoldcmp{x}	compare c-strings using folding case (insensitivity)


Usages:
These subroutines are usedul for those sorting algorithms that
take values or the c-strings.  Thar is: these take pointers
to the character c-strings themselves.  Thsee subroutines
to not take an indirecto pointer to pointers to the c-strings.
Some sorting facilities or containers that take these kind of
subroutines are:

sort(c++)


Note about the |strkeyx...| versions:

These versiona are used on arrays (otherwise often called "vectors")
of pointers to c-strings.  But if the array contains some NULL
pointers, these NULL pointers are sorted to the end of the sorted
array.  This behavior is often wanted (demanded) when it is not
known whether or not NULL pointer are present in the array of
pointers operated on (sorted).


