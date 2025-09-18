/* sigblocker SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* block process signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This was created along with the DATE object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sigblocker

	Description:
	This small object provides a way to block (and unblock)
	process signals.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<ucsig.h>
#include	<localmisc.h>

#include	"sigblocker.h"


/* local defines */


/* external subroutines */


/* exported variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sigblocker_start(sigblocker *op,cint *sigs) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    sigset_t	nsm ;
	    rs = SR_OK ;
	    if (sigs) {
	        if ((rs = uc_sigsetempty(&nsm)) >= 0) {
	            for (int i = 0 ; (rs >= 0) && (sigs[i] > 0) ; i += 1) {
	                rs = uc_sigsetadd(&nsm,sigs[i]) ;
	            }
		}
	    } else {
	        rs = uc_sigsetfill(&nsm) ;
	    }
	    if (rs >= 0) {
	        rs = u_sigmask(SIG_BLOCK,&nsm,&op->osm) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigblocker_start) */

int sigblocker_finish(sigblocker *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = u_sigmask(SIG_SETMASK,&op->osm,nullptr) ;
	}
	return rs ;
}
/* end subroutine (sigblocker_finish) */


/* local subroutines */

int sigblocker_co::operator () (cint *sigs) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    rs = sigblocker_start(op,sigs) ;
	}
	return rs ;
}
/* end method (sigblocker::operator) */

sigblocker_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case sigblockermem_start:
	        rs = sigblocker_start(op,nullptr) ;
	        break ;
	    case sigblockermem_finish:
	        rs = sigblocker_finish(op) ;
	        break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (sigblocker::operator) */

void sigblocker::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("sigblocker",rs,"fini-finish") ;
	}
}


