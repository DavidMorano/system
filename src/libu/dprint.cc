/* dprint SUPPORT (LIBU - Debug-Print) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* version %I% last-modified %G% */
/* C-language defines */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dprintline

	Descrition:
	Print out to debugging a line of text that will be limited in
	length.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD |FILE| + |stderr| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |rmeol(3u)| */
#include	<usysutility.hh>	/* LIBU |snvprintf(3u)| */
#include	<strnul.hh>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"dprint.hh"		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */


using libu::sncpy ;			/* subroutine */
using libu::rmeol ;			/* subroutine */
using libu::snvprintf ;			/* subroutine */


local int fwriter(FILE *,cvoid *,int) noex ;


static FILE	*efp		= stderr ;
cint		maxline		= MAXLINELEN ;


int dprint(cc *func,cc *sp,int µsl) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (int sl ; func && (sl = lenstrline(sp,µsl)) >= 0) {
	    cint pl = rmeol(sp,sl) ;
	    rs = SR_NOMEM ;
	    if (strnul ss(sp,pl) ; ss.fok) {
		ccharp fmt = "%s: %s\n" ;
		rs = fprintf(stderr,fmt,func,ccp(ss)) ;
		len = rs ;
	    } /* end if (strnul) */
	} /* end if (lenstrline) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (dprint) */

int dprintln(cc *func,cc *sp,int sl) noex {
    	return dprint(func,sp,sl) ;
} /* end subroutine (dprintln) */

int dprintstr(cc *func,cc *str,cc *sp,int µsl) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (int sl ; func && str && (sl = lenstrline(sp,µsl)) >= 0) {
	    cint pl = rmeol(sp,sl) ;
	    rs = SR_NOMEM ;
	    if (strnul ss(sp,pl) ; ss.fok) {
		ccharp fmt = "%s: %s%s\n" ;
		rs = fprintf(stderr,fmt,func,str,ccp(ss)) ;
		len = rs ;
	    } /* end if (strnul) */
	} /* end if (lenstrline) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (dprintstr) */

int dprintf(cchar *func,cchar *fmt,...) noex {
    	va_list		ap ;
	cnothrow	nt{} ;
	cint		flen = maxline ;
    	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (func && fmt) ylikely {
	    va_beg(ap,fmt) ;
            rs = SR_NOMEM ;
	    if (char *fbuf = new(nt) char[flen + 1]) ylikely {
		if ((rs = sncpy(fbuf,flen,func,": ")) >= 0) {
		    charp	bp = (fbuf + rs) ;
		    cint	bl = (flen - rs) ;
		    len += rs ;
	            if ((rs = snvprintf(bp,bl,fmt,ap)) >= 0) {
			len += rs ;
		        rs = fwriter(stderr,fbuf,len) ;
	            } /* end if (snvprintf) */
		} /* end if (sncpy) */
		delete [] fbuf ;
	    } /* end if (m-a-f) */
	    va_end(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (dprintf) */


/* local subroutines */

local int fwriter(FILE *fp,cvoid *vap,int µval) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fp && vap) ylikely {
	    cchar *sbuf = charp(vap) ;
	    rs = SR_OK ;
	    if (cint llen = getlenstr(sbuf,µval) ; llen > 0) ylikely {
		csize nsize = size_t(llen) ;
		csize lsize = 1uz ;
	        if (size_t res = fwrite(vap,lsize,nsize,fp) ; res > 0) {
	            wlen += intsat(res) ;
		} else {
		    rs = (errno) ? (neg errno) : SR_OK ;
		} /* end if (error) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (fwriter) */


