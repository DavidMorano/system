/* ncolline SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* calculate number of columns used by a line of characters */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

	= 2025-11-28, David A­D­ Morano
	I have made no modifications, but upon general review I notice
	that this particular subroutine (|ncolline()|) is not used
	anywhere in the current 'system' repository.  If it was used
	in the past (very likely), then for whatever reason that old
	code that used this has been obsoleted (or otherwise somehow
	now gone).  I also note (although not thoroughly examined)
	that this particular subroutine is very similar to the used
	subrtouine |ncols(3uc)|, but each have different signatures.

*/

/* Copyright © 2009,2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ncolline

	Description:
	This subroutine calculates the number of columns that a
	line of characters takes up on a terminal, with tab stops
	set.

	Synopsis:
	int ncolline(int ntab,int ccol,cchar *lbuf,int llen) noex

	Arguments:
	ntab		number of columns in a TAB character
	ccol		current column number
	lbuf		source string pinter
	llen		source string length

	Returns:
	-		column number (after line if used up)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ncol.h>		/* |ncolchar(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"ncolline.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ncolline(int ntab,int ccol,cchar *sp,int µsl) noex {
    	int		rcol = -1 ; /* return-value */
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    rcol = ccol ;
	    for (int i = 0 ; i < sl ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        rcol += ncolchar(ntab,rcol,ch) ;
	    } /* end for */
	} /* end if (getlenstr) */
	return rcol ;
}
/* end subroutine (ncolline) */


