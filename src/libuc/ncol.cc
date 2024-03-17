/* ncol SUPPORT */
/* lang=C++20 */

/* calculate number of columns used by a character */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
	lbuf		line-buffer for given line of characters
	llen		length of line-buffer

	Returns:
	-		number of bytes used for the given number of columns

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<localmisc.h>
#include	<tabcols.h>

#include	"ncol.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ncolstr(int ntab,int ccol,cchar *sp,int sl) noex {
	if (sl < 0) sl = strlen(sp) ;
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

int getcols(int ntab,int ccol,int ncols,cchar *lbuf,int llen) noex {
	cint		tcol = (ccol + ncols) ;
	int		i = 0 ; /* used afterwards */
	if (llen < 0) llen = strlen(lbuf) ;
	if (ccol < tcol) {
	    int		cols ;
	    for (i = 0 ; (ccol < tcol) && (i < llen) ; i += 1) {
	        cols = charcols(ntab,ccol,lbuf[i]) ;
	        ccol += cols ;
	    } /* end for */
	} /* end if */
	return i ;
}
/* end subroutine (getcols) */


