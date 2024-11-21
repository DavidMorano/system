/* sigdefaults SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* set a given set of signals to their default disposition */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sigdefaults

	Description:
	This subroutine sets the disposition of the specified set
	of signals (given as an array of integers containing the
	desired signal numbers) to their default condition.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sigdefaults(int *sigs) noex {
	int		rs = SR_FAULT ;
	if (sigs) {
	    rs = SR_OK ;
	    for (int i = 0 ; (rs >= 0) && (sigs[i] > 0) ; i += 1) {
		rs = uc_sigdefault(sigs[i]) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigdefaults) */


