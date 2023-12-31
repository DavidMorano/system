/* mknpathxw */
/* lang=C++20 */

/* make a file-path from multiple component c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A�D� Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

	= 2011-12-09, David A�D� Morano
	I got rid of the 'strlcpy(3c)' usage.  That subroutine just
	does not represent my moral values!  I now do not know what
	prompted me to do this (probably its extra complexity to
	use).

*/

/* Copyright � 2001,2011 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine constructs a file path out of one or more path
        componets.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>

#include	"mknpathxw.h"


/* local defines */

#define	MKNPATHW_MAXPATHLEN	(4*1024)


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp,MKNPATHW_MAXPATHLEN) ;


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

int mknpathxw(char *pbuf,int plen,int n,...) noex {
	int		rs = SR_FAULT ;
	char		*bp = pbuf ;
	if (pbuf) {
	    va_list	ap ;
	    int		bl = plen ;
	    int		sl = -1 ;
	    va_begin(ap,n) ;
	    rs = SR_OK ;
	    if (plen < 0) {
		rs = maxpathlen ;
		plen = rs ;
	    }
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	        cc	*sp = (cchar *) va_arg(ap,char *) ;
		bool	f = true ;
	        if (i == (n-1)) sl = (int) va_arg(ap,int) ;
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
	            if ((rs = snwcpy(bp,bl,sp,sl)) >= 0) {
	                bp += rs ;
	                bl -= rs ;
	            } else if (rs == SR_OVERFLOW)
	                rs = SR_NAMETOOLONG ;
	        } /* end if */
	    } /* end for */
	    *bp = '\0' ; /* in case of overflow */
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? (bp - pbuf) : rs ;
}
/* end subroutine (mknpathxw) */


