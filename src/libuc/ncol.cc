/* ncol SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate number of columns used by a character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ncolchar

	Description:
	This subroutine calculates the number of columns that a
	character takes up on a terminal with tab stops set.

	Synopsis:
	int ncolchar(int ntab,int ccol,int ch) noex ;

	Arguments:
	ntab		maximum number of columns in a TAB character
	ccol		current column number
	ch		the character to calculate columns for

	Returns:
	-		number of columns used up


	Name:
	ncolstr

	Description:
	This subroutine calculates the number of columns that a
	c-string of characters takes up on a terminal with tab stops
	set.

	Synopsis:
	int ncolstr(int ntab,int ccol,cchar *sp,int sl) noex ;

	Arguments:
	ntab		maximum number of columns in a TAB character
	ccol		current column number
	sp		string buffer
	sl		string length

	Returns:
	-		number of columns used up


	Name:
	getcols

	Description:
	This subroutine returns the number of bytes needed to realize
	the number of columns specified.

	Synopsis:
	int getcols(int ntab,int ccol,int ncols,cchar *lbuf,int llen) noex

	Arguments:
	ntab		number of columns in a TAB character
	ccol		current column number
	ncols		number of additional columns wanted
	lbuf		c-string pointer
	llen		c-string length

	Returns:
	-		number of bytes used for the given number of columns

	Mote:
	It appears that this (above) |getcols| subroutine is not currently
	used anywhere.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<tabcols.h>

#include	"ncol.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ncolstr(int ntab,int ccol,cchar *sp,int sl) noex {
	if (sl < 0) sl = lenstr(sp) ;
	int		cols = 0 ;
	while (sl-- && sp[0]) {
	    cint	ch = mkchar(sp[0]) ;
	    cols += ncolchar(ntab,(ccol+cols),ch) ;
	    sp += 1 ;
	} /* end while */
	return cols ;
}
/* end subroutine (ncolstr) */

int ncolchar(int ntab,int ccol,int ch) noex {
	int		cols = 0 ;
	switch (ch) {
	case CH_SO:
	case CH_SI:
	case CH_SS2:
	case CH_SS3:
	    cols = 0 ;
	    break ;
	case '\t':
	    cols = tabcols(ntab,ccol) ;
	    break ;
	default:
	    if (((ch & 0x7f) >= 0x20) || (ch == 0xff)) {
	        cols = 1 ;
	    }
	    break ;
	} /* end switch */
	return cols ;
}
/* end subroutine (ncolchar) */

int getcols(int ntab,int ccol,int ncols,cchar *sp,int sl) noex {
	cint		tcol = (ccol + ncols) ;
	int		i = 0 ; /* used afterwards */
	if (sl < 0) sl = lenstr(sp) ;
	if (ccol < tcol) {
	    int		cols ;
	    for (i = 0 ; (ccol < tcol) && (i < sl) ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        cols = ncolchar(ntab,ccol,ch) ;
	        ccol += cols ;
	    } /* end for */
	} /* end if */
	return i ;
}
/* end subroutine (getcols) */


