/* tabcols SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate tab columns */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<intceil.h>		/* |iceil(3uc)| */
#include	<localmisc.h>

#include	"tabcols.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int tabcols(int tablen,int ncol) noex {
	cint		n = (ncol % tablen) ;
	return (iceil((n + 1),tablen) - n) ;
}
/* end subroutine (tabcols) */


