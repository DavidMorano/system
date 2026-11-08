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
#include	<strnul.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"dprint.hh"		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */


using libu::rmeol ;			/* subroutine */


static FILE	*efp = stderr ;
cint		llen = MAXLINELEN ;


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

int dprintline(cc *func,cc *sp,int sl) noex {
    	return dprint(func,sp,sl) ;
} /* end subroutine (dprintline) */


