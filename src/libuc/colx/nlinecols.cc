/* nlinecols SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate number of columns used by a line of characters */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nlinecols

	Description:
	This subroutine calculates the number of columns that a
	line of characters takes up on a terminal, with tab stops
	set.

	Synopsis:
	int nlinecols(int ntab,int ccol,cchar *lbuf,int llen) noex

	Arguments:
	ntab		number of columns in a TAB character
	ccol		current column number
	lbuf		source string 
	llen		length of source string

	Returns:
	-		column number after line if used up

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ncol.h>		/* |charcols(3uc)| */
#include	<localmisc.h>

#include	"nlinecols.h"

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

int nlinecols(int ntab,int ccol,cchar *sp,int µsl) noex {
    	int		rcol = -1 ; /* return-value */
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    rcol = ccol ;
	    for (int i = 0 ; i < sl ; i += 1) {
	        rcol += charcols(ntab,rcol,sp[i]) ;
	    } /* end for */
	} /* end if (getlenstr) */
	return rcol ;
}
/* end subroutine (nlinecols) */


