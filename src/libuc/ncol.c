/* ncol */
/* lang=C20 */

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
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


/* exported subroutines */

int ncolstr(int ntab,int ccol,cchar *sp,int sl) noex {
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

int charcols(int ntab,int ccol,int ch) noex {
	return ncolchar(ntab,ccol,ch) ;
}
/* end subroutine (charcols) */



