/* sigignores SUPPORT */
/* lang=C++20 */

/* ignores a group of signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sigignores

	Description:
	This subroutine sets the disposition of the specified set
	of signals (given as an array of integers containing the
	desired signal numbers) such that those signals will be
	ignored.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sigignores(int *sigs) noex {
	int		rs = SR_FAULT ;
	if (sigs) {
	    rs = SR_OK ;
	    for (int i = 0 ; (rs >= 0) && (sigs[i] > 0) ; i += 1) {
		rs = uc_sigignore(sigs[i]) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigignores) */

int ignoresigs(int *sigs) noex {
	return sigignores(sigs) ;
}
/* end subroutine (ignoresigs) */


