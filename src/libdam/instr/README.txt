INSTR

These subroutines perform various actions on c-strings in-place.
That is, they operate on the strings in the same buffers the strings
are located in, rather than forming a result in a separate result
buffer.

subroutine		description
--------------------------------------------------------------------------------

instrcase		case conversion
instrclean		clean a string (or line)
instrcompact		compact a string (removing extra white-space)
instrrev		reverse a string

