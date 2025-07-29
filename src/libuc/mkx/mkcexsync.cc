/* mkcexsync SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make the synchronization string used for CEX */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"mkcexsync.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		reqlen = MKCEXSYNC_REQLEN ;
constexpr int		finlen = MKCEXSYNC_FINLEN ;


/* exported variables */


/* exported subroutines */

int mkcexsync(char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (rbuf) ylikely {
	    cint	leaderlen = (rlen - finlen) ;
	    rs = SR_OVERFLOW ;
	    if (rlen >= reqlen) ylikely {
	        int	j ; /* used-multiple */
	        for (j = (leaderlen-1) ; j >= 0 ; j -= 1) {
	            rbuf[i] = (i & 1) ;
	            i += 1 ;
	        }
	        for (j = 0 ; j < finlen ; j += 1) {
	            rbuf[i++] = CH_SYNC ;
	        }
	        rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkcexsync) */


