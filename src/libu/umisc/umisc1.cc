/* umisc1 MODULE (module-implementation-unit) */
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
	umisc

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlcpy(3c)| */
#include	<cstdarg>		/* |va_list(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

module umisc ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int cstrlcpy(char *dp,cchar *sp,int sl) noex {
    	csize	ssize = size_t(sl) ;
	return int(strlcpy(dp,sp,ssize)) ;
}

static int getrlen(int plen) noex {
    	int		rs = SR_INVALID ;
	if (plen > 0) {
	    rs = plen ;
	}
	return rs ;
} /* end subroutine (getrlen) */


/* local variables */


/* exported variables */


/* exported subroutines */

int mknpath1(char *pp,int pl,cc *s1) noex {
	return mknpathx(pp,pl,1,s1) ;
}
int mknpath2(char *pp,int pl,cc *s1,cc *s2) noex {
	return mknpathx(pp,pl,2,s1,s2) ;
}
int mknpath3(char *pp,int pl,cc *s1,cc *s2,cc *s3) noex {
	return mknpathx(pp,pl,3,s1,s2,s3) ;
}
int mknpath4(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return mknpathx(pp,pl,4,s1,s2,s3,s4) ;
}
int mknpath5(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return mknpathx(pp,pl,5,s1,s2,s3,s4,s5) ;
}
int mknpath6(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6) noex {
	return mknpathx(pp,pl,6,s1,s2,s3,s4,s5,s6) ;
}

int mknpathx(char *pbuf,int plen,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	if (pbuf) {
	    char	*bp = pbuf ;
	    if ((rs = getrlen(plen)) >= 0) {
	        int	rlen = (rs + 1) ;
	        va_begin(ap,n) ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	            cc		*sp = (char *) va_arg(ap,char *) ;
		    bool	f = true ;
	            f = f && (i > 0) && ((bp == pbuf) || (bp[-1] != '/')) ;
		    f = f && (sp[0] != '\0') && (sp[0] != '/') ;
		    if (f) {
	                if (rlen > 1) {		/* must be at least '2' */
	                    *bp++ = '/' ;
	                    rlen -= 1 ;
		        } else {
	                    rs = SR_NAMETOOLONG ;
		        }
	            } /* end if (needed a pathname separator) */
		    if (rs >= 0) {
	                if (int ml ; (ml = cstrlcpy(bp,sp,rlen)) < rlen) {
	        	    bp += ml ;
	        	    rlen -= ml ;
		        } else {
	                    rs = SR_NAMETOOLONG ;
		        }
		    } /* end if */
	        } /* end for */
	        va_end(ap) ;
	    } /* end if (getrlen) */
	    *bp = '\0' ; /* in case of overflow */
	    pl = intconv(bp - pbuf) ;
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mknpathx) */


