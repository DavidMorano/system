STRBCPY (String-Buffer-Copy)

This subroutine concatenates c-strings into a single resulting
destination c-string.  It will not overflow the destiantion character
buffer length.  The result is always NUL terminated (even beyond
the destination character buffer length if necessary).

name		description
--------------------------------------------------------------------------------

strbcpyx	copy strings to a counted destination buffer
strbcpyxc	with case conversion
strbcpyxw	the last source c-string can have a length

