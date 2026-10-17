/* modmisc1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Miscellaneous support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code, originally back around 1992 (of which there
	is quite a bit -- like almost all of it).  I will refrain
	from naming names (at least at this time).

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	modmisc

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

	Notes:
	I am using the new (rumored to be coming as a standard)
	subroutine |strlcpy(3c)|.  I wrote my own version of this
	until it gets into the standard libraries.  It might be in
	someone's standard library already, but I do not have that
	in any of the operating systems I am working with.  And no,
	I do not like the function signature of that new interface
	(API).  I do not like it, but if it is going to become a
	new standard, it might get optimized for speed (like
	writting in hand-coded assembly language).  So that is
	pretty much the only reason I would use that interface
	(which I do not like).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |sncpy(3usys)| */
#include	<localmisc.h>		/* LIBU */

module modmisc ;

/* local defines */


/* imported namespaces */

using usys::sncpy ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int getrlen(int plen) noex {
    	int		rs = SR_INVALID ;
	if (plen > 0) ylikely {
	    rs = plen ;
	} /* end */
	return rs ;
} /* end subroutine (getrlen) */


/* local variables */


/* exported variables */


/* exported subroutines */

int mknpath1(char *pp,int pl,cc *s1) noex {
	return mknpathx(pp,pl,1,s1) ;
} /* end */

int mknpath2(char *pp,int pl,cc *s1,cc *s2) noex {
	return mknpathx(pp,pl,2,s1,s2) ;
} /* end */

int mknpath3(char *pp,int pl,cc *s1,cc *s2,cc *s3) noex {
	return mknpathx(pp,pl,3,s1,s2,s3) ;
} /* end */

int mknpath4(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return mknpathx(pp,pl,4,s1,s2,s3,s4) ;
} /* end */

int mknpath5(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return mknpathx(pp,pl,5,s1,s2,s3,s4,s5) ;
} /* end */

int mknpath6(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return mknpathx(pp,pl,6,s1,s2,s3,s4,s5,s6) ;
} /* end */

int mknpathx(char *pbuf,int µplen,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	if (pbuf) ylikely {
	    if ((rs = getrlen(µplen)) >= 0) ylikely {
		cint plen = rs ;
	        va_begin(ap,n) ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	            ccp  sp = (charp) va_arg(ap,char *) ;
		    if (! sp) break ;
		    rs = pathnadd(pbuf,plen,pl,sp) ;
		    pl = rs ;
	        } /* end for */
	        va_end(ap) ;
	    } /* end if (getrlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (mknpathx) */


