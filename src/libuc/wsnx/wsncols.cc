/* wsncols SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate the number of columns used by a wide-string */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A-D- Morano
	This code was originally written.

	= 2018-10-18, David A-D- Morano
	Fix for proper current column.

*/

/* Copyright © 2009,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wsncols

	Description:
	This subroutine calculates the number of columns that a
	wide-string uses given a horizontal position in a line of
	columns.

	Synopsis:
	int wsncols(int ntab,int scol,const wchar_t *wsp,int wsl) noex

	Arguments:
	ntab		maximum number of columns in a TAB character
	scol		starting current column number
	wsp		wide-string pointer
	wsl		wide-string length (in characters)

	Returns:
	>=0		number of columns used up
	<0		error (null-pointer)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| + |wchar_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>		/* |CH_TAB| */
#include	<tabcols.h>
#include	<localmisc.h>

#include	"wsnx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int wsncolstr(int ntab,int scol,const wchar_t *wsp,int wsl) noex {
    	int		nc = -1 ; /* return-value */
	if (wsp) {
	    int		col = scol ;
	    while (wsl-- && *wsp) {
	        if (*wsp++ == CH_TAB) {
		    col += tabcols(ntab,col) ;
	        } else {
		    col += 1 ;
	        }
	    } /* end while */
	    nc = (col - scol) ;
	} /* end if (non-null) */
	return nc ;
}
/* end subroutine (wsncols) */


