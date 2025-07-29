/* mknpathxw SUPPORT */
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
	mknpathxw

	Description:
        This subroutine constructs a file path out of one or more path
        componets (the last of which can have an optional length).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>		/* |va_list(3c)| */
#include	<cstring>		/* |strlcpy(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"mknpathxw.h"


/* local defines */

#define	MKNPATHW_MAXPATHLEN	(4*1024)


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct maxpather {
	static int operator () (int) noex ;
    } ; /* end struct (maxpather) */
}


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp,MKNPATHW_MAXPATHLEN) ;
static maxpather	getrlen ;


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
	if (pbuf) ylikely {
	    if ((rs = getrlen(plen)) >= 0) ylikely {
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
	            if (rs >= 0) ylikely {
	                if ((rs = snwcpy(bp,bl,sp,sl)) > 0) ylikely {
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
	    } /* end if (getplen) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mknpathxw) */


/* local subroutines */

int maxpather::operator () (int plen) noex {
    	int		rs ;
	if ((rs = plen) < 0) ylikely {
	    rs = maxpathlen ;
	}
	return rs ;
}
/* end method (maxpather::operator) */


