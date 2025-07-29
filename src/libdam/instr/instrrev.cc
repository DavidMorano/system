/* mkrevstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* reverse the characters in a string in place */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkrevstr

	Description:
	Reverse the characters of a string in place.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>

#include	"instrrev.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkrevstr(char *bp,int bl) noex {
	int		rs = SR_FAULT ;
	if (bp) ylikely {
	    if (bl < 0) bl = lenstr(bp) ;
	    for (int i = 0 ; i < (bl/2) ; i += 1) {
	        int	ch = bp[i] ;
	        bp[i] = bp[bl-i-1] ;
	        bp[bl-i-1] = char(ch) ;
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (mkrevstr) */


