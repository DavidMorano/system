SHOWFIELDTERMS

This program is used to show the terminator characters that are
making up a field terminator character block.  These field terminator
blocks are used in subroutine like:

	field_get(3uc)
	field_sharg(3uc)
	field_srvarg(3uc)
	field_word(3uc)
	field_wordphrase(3uc)

The ASCII representation of the terminator block should be read in
on either a file named by the first positional argument of the
program or by STDIN of this program.

Synopsis:
$ showfieldterms <termfile> [-V]

Arguments:
<termfile>	file containing the ASCII representation of field terminators
-V		print program version to standard-error and then exit

