LIBCX

This is a UNIX® interface library.  This library provides some
function interfaces that might be missing in the standard distribution
libraries.  Currently we only make up for some missing library calls
in Sun (Oracle) Solaris®.  These are:

	pipe2(2)
	strtoimax(3c)
	strtoumax(3c)
	logbase(3c)
	log2(3c)
	eaccess(3c)

This libary should be followed by linking in the LIBUC library
(in order to supply the symbol |perm(3uc)|, and possibly some
others).


