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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |COLUMNS| */

#include	"debugutil.hh"
#include	"libdebug.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |char_toval(3u)| */

/* local defines */

#define	PRINTBUFLEN	(COLUMNS + 2)


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
	charp		s ;
} ; /* end struct (debug_oflags) */


/* forward subroutines */

local char	getdig(int v) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mkhexstr(char *dbuf,int dlen,cvoid *vp,int vl) noex {
    	int		rs = SR_FAULT ;
	int		j = 0 ; /* return-value */
	if (dbuf && vp) ylikely {
	    int		sl = vl ;
	    cchar	*sp = ccharp(vp) ;
	    if (sl < 0) sl = lenstr(sp) ;
	    rs = SR_OK ;
	    for (int i = 0 ; (dlen >= 3) && (i < sl) ; i += 1) {
	        cint ch = mkchar(sp[i]) ;
	        if (i > 0) dbuf[j++] = ' ' ;
	        dbuf[j++] = getdig((ch>>4)&15) ;
	        dbuf[j++] = getdig((ch>>0)&15) ;
	        dlen -= ((i > 0) ? 3 : 2) ;
	    } /* end for */
	    dbuf[j] = eos ;
	} /* end if (non-null) */
	return (rs >= 0) ? j : rs ;
} /* end subroutine (mkhexstr) */

int mkhexnstr(char *hbuf,int hlen,int maxcols,cchar *sbuf,int slen) noex {
    	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (hbuf) ylikely {
	    if (maxcols < 0) maxcols = COLUMNS ;
	    if (slen < 0) slen = lenstr(sbuf) ;
	    n = min((maxcols / 3),slen) ;
	    rs =  mkhexstr(hbuf,hlen,sbuf,n) ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (mkhexnstr) */

int mkhexblock(cchar *ids,cchar *ap,int n) noex {
    	int		rs = SR_FAULT ;
	cint		hexlen = HEXBUFLEN ;
	char		hexbuf[HEXBUFLEN + 3] ;
	if (ap) ylikely {
	    rs = SR_OK ;
	    if (ids) {
	        rs = debugprint(ids,-1) ;
	    } /* end if */
	    for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
		if ((rs = mkhexstr(hexbuf,hexlen,ap,4)) >= 0) {
	            int sl = rs ;
	            hexbuf[sl++]	= eol ;
	            hexbuf[sl]		= eos ;
	            ap += 4 ;
	            rs = debugprint(hexbuf,-1) ;
		} /* end if (mkhexstr) */
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (mkhexblock) */

local char getdig(int v) noex {
    	char	c = '¿' ;
	if ((v >= 0) && (v < 10)) {
	    c = '0' + char(v) ;
	} else if ((v >= 10) && (v < 16)) {
	    c = 'a' + char(v) ;
	} /* end if */
	return c ;
} /* end subroutine (getdig) */


