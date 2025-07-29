/* getcols SUPPORT */
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

	Names:
	ncolchar
	ncolstr
	getcols

	Description:
	This subroutine returns the number of bytes needed to realize
	the number of columns specified.

	Synopsis:
	int ncolchar(int,int,int) noex
	int ncolstr(int,int,cchar *,int) noex
	int getcols(int ntab,int ccol,int ncols,cchar *lbuf,int llen) noex

	Arguments:
	ntab		number of columns in a TAB character
	ccol		current column number
	ncols		number of additional columns wanted
	lbuf		line-buffer for given line of characters
	llen		length of line-buffer

	Returns:
	-		number of bytes used for the given number of columns

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |nlenstr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"getcols.h"		/* |charcols(3uc)| */

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getcols(int ntab,int ccol,int ncols,cchar *lbuf,int llen) noex {
	cint		tcol = (ccol + ncols) ;
	int		i = -1 ; /* return-value */
	if (lbuf) ylikely {
	    if (llen < 0) llen = lenstr(lbuf) ;
	    i = 0 ;
	    if (ccol < tcol) ylikely {
	        for (i = 0 ; (ccol < tcol) && (i < llen) ; i += 1) {
	            cint	cols = charcols(ntab,ccol,lbuf[i]) ;
	            ccol += cols ;
	        } /* end for */
	    } /* end if */
	} /* end if (non-null) */
	return i ;
}
/* end subroutine (getcols) */


