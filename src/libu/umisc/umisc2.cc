/* umisc2 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Miscellaneous support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-19, David A­D­ Morano
	This code was originally written.

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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>		/* |snwcpy(3u)| */
#include	<localmisc.h>

module umisc ;

using libu::snwcpy ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

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

int mknpath1w(char *pp,int pl,cc *s1,int sl) noex {
	return mknpathxw(pp,pl,1,s1,sl) ;
}
/* end subroutine (mknpath1w) */

int mknpath2w(char *pp,int pl,cc *s1,cc *s2,int sl) noex {
	return mknpathxw(pp,pl,2,s1,s2,sl) ;
}
/* end subroutine (mknpath2w) */

int mknpath3w(char *pp,int pl,cc *s1,cc *s2,cc *s3,int sl) noex {
	return mknpathxw(pp,pl,3,s1,s2,s3,sl) ;
}
/* end subroutine (mknpath3w) */

int mknpath4w(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return mknpathxw(pp,pl,4,s1,s2,s3,s4,sl) ;
}
/* end subroutine (mknpath4w) */

int mknpath5w(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl) noex {
	return mknpathxw(pp,pl,5,s1,s2,s3,s4,s5,sl) ;
}
/* end subroutine (mknpath5w) */

int mknpath6w(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,
		int sl) noex {
	return mknpathxw(pp,pl,6,s1,s2,s3,s4,s5,s6,sl) ;
}
/* end subroutine (mknpath6w) */

int mknpathxw(char *pbuf,int plen,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	char		*bp = pbuf ;
	if (pbuf) {
	    if ((rs = getrlen(plen)) >= 0) {
	        int	bl = rs ;
	        int	sl = -1 ;
	        va_begin(ap,n) ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	            cc		*sp = (cchar *) va_arg(ap,char *) ;
		    bool	f = true ;
	            if (i == (n - 1)) sl = (int) va_arg(ap,int) ;
	            f = f && (i > 0) && ((bp == pbuf) || (bp[-1] != '/')) ;
		    f = f && (sp[0] != '\0') && (sp[0] != '/') ;
		    if (f) {
	                if (bl > 0) {
	                    *bp++ = '/' ;
	                    bl -= 1 ;
	                } else {
	                    rs = SR_NAMETOOLONG ;
		        }
	            } /* end if (needed a pathname separator) */
	            if (rs >= 0) {
	                if ((rs = snwcpy(bp,bl,sp,sl)) > 0) {
	                    bp += rs ;
	                    bl -= rs ;
	                } else if (rs == SR_OVERFLOW) {
	                    rs = SR_NAMETOOLONG ;
		        }
	            } /* end if */
	        } /* end for */
	        *bp = '\0' ; /* in case of overflow */
		pl = intconv(bp - pbuf) ;
	        va_end(ap) ;
	    } /* end if (getrlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mknpathxw) */


