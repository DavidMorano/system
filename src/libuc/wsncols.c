/* wsncols */
/* lang=C20 */

/* calculate the number of columns used by a wide-string */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-04-10, David A-D- Morano
	This subroutine was originally written.

	= 2018-10-18, David A-D- Morano
	Fix for proper current column.

*/

/* Copyright © 2009,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wsncols

	Description:
	This subroutine calculates the number of columns that a wide-string
	uses given a horizontal position in a line of columns.

	Synopsis:
	int wsncols(int ntab,int scol,const wchar_t *wsp,int wsl) noex

	Arguments:
	ntab		maximum number of columns in a TAB character
	scol		starting current column number
	wsp		wide-string pointer
	wsl		wide-string length (in characters)

	Returns:
	-		number of columns used up


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stddef.h>		/* for 'wchar_t' */
#include	<ascii.h>
#include	<localmisc.h>
#include	<tabcols.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int wsncolstr(int ntab,int scol,const wchar_t *wsp,int wsl) noex {
	int		col = scol ;
	while (wsl-- && *wsp) {
	    if (*wsp++ == CH_TAB) {
		col += tabcols(ntab,col) ;
	    } else {
		col += 1 ;
	    }
	} /* end while */
	return (col-scol) ;
}
/* end subroutine (wsncols) */


