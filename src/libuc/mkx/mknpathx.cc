/* mknpathx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make a file path from components */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code, originally back around 1992 (of which there
	is quite a bit -- like almost all of it).  I will refrain
	from naming names (at least at this time).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine constructs a file path out of one or more path
	componets.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>

#include	"mknpathx.h"


/* local defines */

#define	MKNPATH_MAXPATHLEN	(4*1024)	/* default value (only) */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp,MKNPATH_MAXPATHLEN) ;


/* exported variables */


/* exported subroutines */

int mknpath1(char *pp,int pl,cc *s1) noex {
	return mknpathx(pp,pl,1,s1) ;
}
/* end subroutine (mknpath1) */

int mknpath2(char *pp,int pl,cc *s1,cc *s2) noex {
	return mknpathx(pp,pl,2,s1,s2) ;
}
/* end subroutine (mknpath2) */

int mknpath3(char *pp,int pl,cc *s1,cc *s2,cc *s3) noex {
	return mknpathx(pp,pl,3,s1,s2,s3) ;
}
/* end subroutine (mknpath3) */

int mknpath4(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4) noex {
	return mknpathx(pp,pl,4,s1,s2,s3,s4) ;
}
/* end subroutine (mknpath4) */

int mknpath5(char *pp,int pl,cc *s1,cc *s2,cc *s3,cc *s4,cc *s5) noex {
	return mknpathx(pp,pl,5,s1,s2,s3,s4,s5) ;
}
/* end subroutine (mknpath5) */

int mknpathx(char *pbuf,int plen,int n,...) noex {
	int		rs = SR_FAULT ;
	char		*bp = pbuf ;
	if (pbuf) {
	    int		ml = 0 ;
	    rs = SR_OK ;
	    if (plen < 0) {
		rs = maxpathlen ;
		plen = rs ;
	    }
	    if (rs >= 0) {
	        va_list		ap ;
	        int		rlen = (plen + 1) ;
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
	                if ((ml = strlcpy(bp,sp,rlen)) < rlen) {
	        	    bp += ml ;
	        	    rlen -= ml ;
		        } else {
	                    rs = SR_NAMETOOLONG ;
		        }
		    } /* end if */
	        } /* end for */
	        va_end(ap) ;
	    } /* end block */
	    *bp = '\0' ; /* in case of overflow */
	} /* end if (non-null) */
	return (rs >= 0) ? (bp - pbuf) : rs ;
}
/* end subroutine (mknpathx) */


