/* mkcexsync SUPPORT */
/* lang=C++20 */

/* make the synchronization string used for CEX */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1992 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkcexsync

	Description:
	This subroutine creates the synchronization string sequence
	for use by CEX.

	Synopsis:
	int mkcexsync(char *rbuf,int rlen) noex

	Arguments:
	rbuf		host to dial to
	rlen		length of buffer (really just a check)

	Returns:
	>=0		length of resulting sequence
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<ascii.h>

#include	"mkcexsync.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkcexsync(char *rbuf,int rlen) noex {
	cint		leaderlen = (rlen - MKCEXSYNC_FINLEN) ;
	int		rs = SR_OVERFLOW ;
	int		i = 0 ;
	if (rlen >= MKCEXSYNC_REQLEN) {
	    int		j{} ;
	    for (j = (leaderlen-1) ; j >= 0 ; j -= 1) {
	        rbuf[i] = (i & 1) ;
	        i += 1 ;
	    }
	    for (j = 0 ; j < MKCEXSYNC_FINLEN ; j += 1) {
	        rbuf[i++] = CH_SYNC ;
	    }
	    rs = SR_OK ;
	} /* end if (valid) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkcexsync) */

