/* sigblocker */
/* lang=C++20 */

/* block process signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This was created along with the DATE object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This small object provides a way to block (and unblock)
	process signals.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<signal.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"sigblocker.h"


/* local defines */


/* external subroutines */


/* exported variables */


/* local subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int sigblocker_start(SIGBLOCKER *op,const int *sigs) noex {
	int		rs = SR_FAULT ;
	if (op) {
	sigset_t	nsm ;
	    rs = SR_OK ;
	    if (sigs) {
	        uc_sigsetempty(&nsm) ;
	        for (int i = 0 ; (rs >= 0) && (sigs[i] > 0) ; i += 1) {
	            rs = uc_sigsetadd(&nsm,sigs[i]) ;
	        }
	    } else {
	        rs = uc_sigsetfill(&nsm) ;
	    }
    
	    if (rs >= 0) {
	        rs = pt_sigmask(SIG_BLOCK,&nsm,&op->osm) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigblocker_start) */

int sigblocker_finish(SIGBLOCKER *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pt_sigmask(SIG_SETMASK,&op->osm,NULL) ;
	}
	return rs ;
}
/* end subroutine (sigblocker_finish) */


/* local subroutines */

int sigblocker_co::operator () (int *sigs) noex {
	int	rs = SR_BUGCHECK ;
	if (op) {
	    rs = sigblocker_start(op,sigs) ;
	}
	return rs ;
}
/* end method (sigblocker::operator) */

sigblocker_co::operator int () noex {
	int	rs = SR_BUGCHECK ;
	switch (w) {
	case sigblockermem_start:
	    rs = sigblocker_start(op,nullptr) ;
	    break ;
	case sigblockermem_finish:
	    rs = sigblocker_finish(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (sigblocker::operator) */


