/* tabcols */
/* lang=C20 */

/* calculate tab columns */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tabcols

	Description:
	This small subroutine calculates the number of columns that
	would be skipped by advancing to the next tab-stop.

	Synopsis:
	int tabcols(int tablen,int ncol) noex
	
	Arguments:
	tablen		number of columns in a tab
	ncol		current column number (counting from zero)

	Returns:
	-		number of columns that would be moved forward

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<intceil.h>

#include	"tabcols.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int tabcols(int tablen,int ncol) noex {
	cint		n = (ncol % tablen) ;
	return (iceil((n + 1),tablen) - n) ;
}
/* end subroutine (tabcols) */


