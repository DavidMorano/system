/* mkrevstr SUPPORT */
/* lang=C++20 */

/* reverse the characters in a string in place */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Reverse the characters of a string in place.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<ascii.h>


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
	if (bp) {
	    if (bl < 0) bl = strlen(bp) ;
	    for (int i = 0 ; i < (bl/2) ; i += 1) {
	        int	ch = bp[i] ;
	        bp[i] = bp[bl-i-1] ;
	        bp[bl-i-1] = ch ;
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (mkrevstr) */

