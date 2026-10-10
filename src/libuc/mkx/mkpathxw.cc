/* mkpathxw SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a file-path from multiple component c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

	= 2011-12-09, David A­D­ Morano
	I got rid of the |strlcpy(3c)| usage.  That subroutine just
	does not represent my moral values!  I now do not know what
	prompted me to do this (probably its extra complexity to
	use).

*/

/* Copyright © 1998,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Family:
	mkpathxw

	Description:
        This subroutine constructs a file path out of one or more path
        componets.

	Notes (obsoleted 2011-12-09, see revision above):
	I am using the new (rumored to be coming as a standard)
	subroutine |strlcpy(3c)|.  I wrote my own version of this
	until it gets into the standard libraries.  It might be in
	someone's standard lirbrary already, but I do not have that
	in any of the operating systems I am working with.  And no,
	I do not like the function signature of that new interface
	(API).  I do not like it, but if it is going to become a
	new standard, it might gets optimized for speed (like
	writting in hand-coded assembly language).  So that is
	pretty much the only reason I would use that interface
	(which I do not like).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mkpathxw.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int mknpathxw(char *,int,int,...) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkpath1w(char *pp,cc *s1,int sl) noex {
	return mknpathxw(pp,-1,1,s1,sl) ;
} /* end subroutine (mknpath1w) */

int mkpath2w(char *pp,cc *s1,cc *s2,int sl) noex {
	return mknpathxw(pp,-1,2,s1,s2,sl) ;
} /* end subroutine (mknpath2w) */

int mkpath3w(char *pp,cc *s1,cc *s2,cc *s3,int sl) noex {
	return mknpathxw(pp,-1,3,s1,s2,s3,sl) ;
} /* end subroutine (mknpath3w) */

int mkpath4w(char *pp,cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return mknpathxw(pp,-1,4,s1,s2,s3,s4,sl) ;
} /* end subroutine (mknpath4w) */

int mkpath5w(char *pp,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl) noex {
	return mknpathxw(pp,-1,5,s1,s2,s3,s4,s5,sl) ;
} /* end subroutine (mknpath5w) */

int mkpath6w(char *pp,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int sl) noex {
	return mknpathxw(pp,-1,6,s1,s2,s3,s4,s5,s6,sl) ;
} /* end subroutine (mknpath6w) */


