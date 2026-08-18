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
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"sigblocker.h"

import usigsets ;			/* signal-sets */

/* local defines */


/* external subroutines */


/* exported variables */


/* local structures */


/* forward references */

local int	mksignew(sigset_t *,cint *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sigblocker_start(sigblocker *op,cint *sigs) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (sigset_t nsm{} ; (rs = mksignew(&nsm,sigs)) >= 0) ylikely {
		cint cmd = SIG_BLOCK ;
		if (sigset_t o ; (rs = u_sigmask(cmd,&nsm,&o)) >= 0) ylikely {
		    op->osm = o ;
		    op->magval = SIGBLOCKER_MAGIC ;
		} /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sigblocker_start) */

int sigblocker_finish(sigblocker *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = u_sigmask(SIG_SETMASK,&op->osm,nullptr) ;
	    op->magval = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sigblocker_finish) */


/* local subroutines */

int sigblocker_co::operator () (cint *sigs) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = sigblocker_start(op,sigs) ;
	}
	return rs ;
} /* end method (sigblocker::operator) */

sigblocker_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case sigblockermem_start:
	        rs = sigblocker_start(op,nullptr) ;
	        break ;
	    case sigblockermem_finish:
	        rs = sigblocker_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (sigblocker::operator) */

void sigblocker::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("sigblocker",rs,"fini-finish") ;
	}
} /* end method (sigblocker::dtor) */

local int mksignew(sigset_t *rp,cint *sigs) noex {
    	int		rs = SR_OK ;
	if (sigs) {
	    if ((rs = usigset.empty(rp)) >= 0) {
		for (int i = 0 ; (rs >= 0) && (sigs[i] > 0) ; i += 1) {
		    rs = usigset.add(rp,sigs[i]) ;
		} /* end for */
	    } /* end if */
	} else {
	    rs = usigset.fill(rp) ;
	} /* end if */
	return rs ;
} /* end subroutine (mksignew) */


