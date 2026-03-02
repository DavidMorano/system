/* debugmkhex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debugging stubs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-15, David A­D­ Morano
	This was written to debug the REXEC program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	debugmkhex{x}

	Description:
	This modeule provides debugging support.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* |COLUMNS| */

#include	"debugutil.hh"
#include	"libdebug.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |char_toval(3u)| */

/* local defines */

#define	PRINTBUFLEN	(COLUMNS + 2)

#ifndef	HEXBUFLEN
#define	HEXBUFLEN	100
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libdebug::checkbasebounds ;	/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct debug_oflags {
	int		m ;
	char		*s ;
} ; /* end struct (debug_oflags) */


/* forward subroutines */

local char	getdig(int v) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mkhexstr(char *dbuf,int dlen,cvoid *vp,int vl) noex {
	int		j = 0 ;
	if (dbuf && vp) {
	    int		sl = vl ;
	    int		ch ;
	    cchar	*sp = ccharp(vp) ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (int i = 0 ; (dlen >= 3) && (i < sl) ; i += 1) {
	        ch = mkchar(sp[i]) ;
	        if (i > 0) dbuf[j++] = ' ' ;
	        dbuf[j++] = getdig((ch>>4)&15) ;
	        dbuf[j++] = getdig((ch>>0)&15) ;
	        dlen -= ((i > 0) ? 3 : 2) ;
	    } /* end for */
	    dbuf[j] = '\0' ;
	} /* end if (non-null) */
	return j ;
}
/* end subroutine (mkhexstr) */

int mkhexnstr(char *hbuf,int hlen,int maxcols,cchar *sbuf,int slen) noex {
    	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (hbuf) {
	    if (maxcols < 0) maxcols = COLUMNS ;
	    if (slen < 0) slen = lenstr(sbuf) ;
	    n = min((maxcols / 3),slen) ;
	    rs =  mkhexstr(hbuf,hlen,sbuf,n) ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkhexnstr) */

int mkhexblock(cchar *ids,cchar *ap,int n) noex {
	cint		hexlen = HEXBUFLEN ;
	char		hexbuf[HEXBUFLEN + 3] ;
	if (ids) {
	    debugprint(ids,-1) ;
	}
	for (int i = 0 ; i < n ; i += 1) {
	    int sl = mkhexstr(hexbuf,hexlen,ap,4) ;
	    hexbuf[sl++] = '\n' ;
	    hexbuf[sl] = '\0' ;
	    ap += 4 ;
	    debugprint(hexbuf,-1) ;
	} /* end for */
	return n ;
}
/* end subroutine (mkhexblock) */

local char getdig(int v) noex {
    	char	c = '¿' ;
	if ((v >= 0) && (v < 10)) {
	    c = '0' + char(v) ;
	} else if ((v >= 10) && (v < 16)) {
	    c = 'a' + char(v) ;
	}
	return c ;
} /* end subroutine (getdig) */


