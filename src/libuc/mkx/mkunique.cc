/* mkunique SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++11 */

/* test whether a string consists of all unique characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkunique

	Description:
	Create a new c-string (in the given result buffer) that is
	a copy of the source c-string but with all duplicated
	characters removed.

	Synopsis:
	int mkunique(char *rbuf,int rlen,cchar *sp,int sl) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	sp		source test-string pointer
	sl		source test-string length

	Returns:
	>=0		length of given string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bitset>		/* C++STD |bitset(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strmgr.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mkunique.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

using std::bitset ;			/* type */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		nchars = (UCHAR_MAX + 1) ;


/* exported variables */


/* exported subroutines */

int mkunique(char *rbuf,int rlen,cchar *sp,int µsl) noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen >= 0) {
		rs = SR_OK ;
		if (int sl = getlenstr(sp,µsl) ; sl > 0) ylikely {
		    if (strmgr m ; (rs = m.start(rbuf,rlen)) >= 0) {
	                bitset<nchars>	seen ;
			for (int ch ; sl-- && ((ch = mkchar(*sp))) ; ++sp) {
		            if (! seen[ch]) {
		                rs = m.chr(ch) ;
	                        seen[ch] = true ;
		            }
			    if (rs < 0) break ;
	                } /* end for */
		        rl = m.finish ;
		        if (rs >= 0) rs = rl ;
	            } /* end if (strmgr) */
	        } /* end if (getlenstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (mkunique) */


