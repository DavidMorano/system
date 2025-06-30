/* uobjlock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Object-Lock-Management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uobjlock

	Description:
	This object helps manage the locking issues related to
	emulated kernel calls.  The idea is that objects inherit
	from this object in order to get fork-safety for things
	like emulated kernel calls.  Making something fork-safe is
	not really easy, so I am trying to help the poor developers
	(like myself) who need something (like an emulated kernel)
	call to be fork-safe without having to do though the full
	monty in order to get it.  Thead-safety is not really enough
	for kernel calls.  I wish that it was, but it is not.

	Notes:
	1. This object is fork-safe.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* |mode_t| */
#include	<csignal>		/* |sig_atomic_t| */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<uregfork.hh>		/* |uregfork{x}(3u)| */
#include	<aflag.hh>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<ulogerror.h>
#include	<localmisc.h>

#include	"uobjlock.hh"

import usysbasic ;

/* local defines */


/* imported namespaces */

using libu::uregfork_rec ;		/* subroutine */
using libu::uregfork_exp ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external veriables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uobjlock::iinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    rs = SR_INVALID ;
	    if (afbefore_f || afparent_f || afchild_f) {
	        cint	to = utimeout[uto_busy] ;
	        rs = SR_OK ;
	        if (! finit.testandset) {
	            if ((rs = mx.create) >= 0) {
	                void_f	fbef = afbefore_f ;
	                void_f	fpar = afparent_f ;
	                void_f	fchi = afchild_f ;
	                if ((rs = uregfork_rec(fbef,fpar,fchi)) >= 0) {
	                    if ((rs = isetexit()) >= 0) {
			        rs = initonce() ;
	    	                finitdone = true ;
		                f = true ;
		            } /* end if (isetexit) */
		            if (rs < 0) {
		                uregfork_exp(fbef,fpar,fchi) ;
			    }
	                } /* end if (uregfork) */
	 	        if (rs < 0) {
		            mx.destroy() ;
		        }
	            } /* end if (ptm_create) */
	            if (rs < 0) {
	                finit = false ;
		    }
                } else if (! finitdone) { 
                    timewatch       tw(to) ;
                    auto lamb = [this] () -> int {
                        int         rsl = SR_OK ;
                        if (!finit) {
                            rsl = SR_LOCKLOST ;              /* <- failure */
                        } else if (finitdone) {
                            rsl = 1 ;                        /* <- OK ready */
                        }                       
                        return rsl ;
                    } ; /* end lambda (lamb) */ 
                    rs = tw(lamb) ;         /* <- time-watching */
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uobjlock::iinit) */

int uobjlock::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
		rs1 = finionce() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	fbef = afbefore_f ;
	        void_f	fpar = afparent_f ;
	        void_f	fchi = afchild_f ;
	        rs1 = uregfork_exp(fbef,fpar,fchi) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (atexit registered) */
	return rs ;
}
/* end method (uobjlock::ifini) */

int uobjlock::icallbegin(int to) noex {
	int		rs ;
	if ((rs = init) >= 0) {
	    if ((rs = mx.lockbegin(to)) >= 0) {
		rs = setupbegin() ;
	    }
	}
	return rs ;
} /* end method (uobjlock::icallbegin) */

int uobjlock::icallend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = setupend() ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end method (uobjlock::icallend) */

int uobjlock::isetexit() noex {
    	int		rs = SR_OK ;
	if (aefunc_f) {
	    rs = uatexit(aefunc_f) ;
	} /* end if (non-null) */
	return rs ;
}
/* end method (uobjlock::isetexit) */

void uobjlock::dtor() noex {
	if (cint rs = ifini() ; rs < 0) {
            ulogerror("uobjlock",rs,"dtor-fini") ;
	}
} /* end method (uobjlock::dtor) */

int uobjlock_co::operator () (int to) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case uobjlockmem_init:
	        rs = op->iinit() ;
	        break ;
	    case uobjlockmem_fini:
	        rs = op->ifini() ;
	        break ;
	    case uobjlockmem_callbegin:
	        rs = op->icallbegin(to) ;
	        break ;
	    case uobjlockmem_callend:
	        rs = op->icallend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (uobjlock_co::operator) */


