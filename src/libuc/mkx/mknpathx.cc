/* mknpathx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a file path from components */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code, originally back around 1992 (of which there
	is quite a bit -- like almost all of it).  I will refrain
	from naming names (at least at this time).

	= 2026-10-15, David A­D­ Morano
	I am getting rid of the use of the |strlcpy(3c)| subroutine.
	Before it even became available in standard system (C-language)
	libraries, I was using my own implementaton of that subroutine.
	Why am I doing this?   The truth is that I do not have a
	really good reason.  It lasted thse last couple or few
	decades, so why amy I doing this now.  I do not really know
	why.  I just want to get rid of it as a dependency.  I never
	really liked its function-signature either.  Actually, I
	always hated its function-signature.  I never liked its
	behavior also.  It reads the whole c-string pased no matter
	how long it may be.  Although that behavior is not a problem
	in its use below, it is indeed a bad behavior elsewhere.
	So goodbye to that POS.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Family:
	mknpathx

	Description:
	This subroutine constructs a file path out of one or more path
	componets.

	Notes: (obsoleted 2026-10-15, see revision note above)
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

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC |sncpy(3uc)| */
#include	<snaddslash.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mknpathx.h"


/* local defines */

#define	MKNPATH_MAXPATHLEN	(4*1024)	/* default value (only) */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct maxpather {
	local int operator () (int) noex ;
    } ; /* end struct (maxpather) */
} /* end namespace */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(bufsize_mp,MKNPATH_MAXPATHLEN) ;
static maxpather	getrlen ;


/* exported variables */


/* exported subroutines */

int mknpath1(char *pp,int pl,cc *s1) noex {
	return mknpathx(pp,pl,1,s1) ;
} /* end subroutine (mknpath1) */

int mknpath2(char *pp,int pl,cc *s1,cc *s2) noex {
	return mknpathx(pp,pl,2,s1,s2) ;
} /* end subroutine (mknpath2) */

int mknpath3(char *pp,int pl,cc *s1,cc *s2,cc *s3) noex {
	return mknpathx(pp,pl,3,s1,s2,s3) ;
} /* end subroutine (mknpath3) */

int mknpath4(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return mknpathx(pp,pl,4,s1,s2,s3,s4) ;
} /* end subroutine (mknpath4) */

int mknpath5(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return mknpathx(pp,pl,5,s1,s2,s3,s4,s5) ;
} /* end subroutine (mknpath5) */

int mknpath6(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return mknpathx(pp,pl,6,s1,s2,s3,s4,s5,s6) ;
} /* end subroutine (mknpath5) */

int mknpathx(char *pbuf,int plen,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	if (pbuf) ylikely {
	    charp bp = pbuf ;
	    if ((rs = getrlen(plen)) >= 0) ylikely {
		cint blen = rs ;
		int  bl = rs ;
	        va_begin(ap,n) ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		    cint rl = intconv(bp - pbuf) ;
	            ccp	 sp = (charp) va_arg(ap,char *) ;
		    if (! sp) break ;
		    if ((rs = snaddslash(pbuf,blen,rl)) >= 0) {
			bp += rs ;
			bl -= rs ;
	                if ((rs = sncpy(bp,bl,sp)) >= 0) {
	                    bp += rs ;
			    bl -= rs ;
			} /* end if (sncpy) */
		    } /* end if (ok) */
	        } /* end for */
	        va_end(ap) ;
	    } /* end if (getrlen) */
	    *bp = eos ; /* in case of overflow */
	    if (rs == SR_OVERFLOW) rs = SR_NAMETOOLONG ;
	    pl = intconv(bp - pbuf) ;
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (mknpathx) */


/* local subroutines */

int maxpather::operator () (int plen) noex {
    	int		rs ;
	if ((rs = plen) < 0) ylikely {
	    rs = maxpathlen ;
	} /* end */
	return rs ;
} /* end method (maxpather::operator) */


