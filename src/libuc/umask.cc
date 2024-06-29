/* umask SUPPORT */
/* lang=C++20 */

/* UNIX® UMASK (file-creation-mask) management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David AÂ­DÂ­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module serves to provide two functions that manipulate
	the UNIX® UMASK.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<csignal>		/* |sig_atomic_t| */
#include	<climits>		/* |INT_MAX| */
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<aflag.hh>
#include	<timewatch.hh>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<localmisc.h>

#include	"umask.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
    struct umasker {
	ptm		mx ;		/* data mutex */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init() noex ;
	int fini() noex ;
	int get() noex ;
	int setmode(mode_t) noex ;
	void forkbefore() noex {
	    mx.lockbegin() ;
	} ;
	void forkafter() noex {
	    mx.lockend() ;
	} ;
	~umasker() {
            cint        rs = fini() ;
            if (rs < 0) {
                ulogerror("umask",rs,"dtor-fini") ;
            }
	} ;
    } ; /* end struct (umasker) */
}


/* forward references */

extern "C" {
    static void	umask_atforkbefore() noex ;
    static void	umask_atforkafter() noex ;
    static void	umask_exit() noex ;
}


/* local variables */

static umasker	umask_data ;


/* exported variables */


/* exported subroutines */

int umask_init() noex {
	return umask_data.init() ;
}
/* end subroutine (umask_init) */

int umask_fini() noex {
	return umask_data.fini() ;
}
/* end subroutine (umask_fini) */

int umaskget() noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		cmask = 0 ;
	if ((rs = b.start) >= 0) {
	    {
		rs = umask_data.get() ;
		cmask = rs ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? cmask : rs ;
}
/* end subroutine (umaskget) */

int umaskset(mode_t cmask) noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	if ((rs = sigblocker_start(&b,nullptr)) >= 0) {
	    {
		rs = umask_data.setmode(cmask) ;
		omask = rs ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? omask : rs ;
}
/* end subroutine (umaskset) */


/* local subroutines */

int umasker::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = umask_atforkbefore ;
	            void_f	a = umask_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(umask_exit)) >= 0) {
	    	            finitdone = true ;
		            f = true ;
		        }
		        if (rs < 0) {
		            uc_atforkexpunge(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
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
                    int         rs = SR_OK ;
                    if (!finit) {
                        rs = SR_LOCKLOST ;              /* <- failure */
                    } else if (finitdone) {
                        rs = 1 ;                        /* <- OK ready */
                    }                       
                    return rs ;
                } ; /* end lambda (lamb) */ 
                rs = tw(lamb) ;         /* <- time-watching occurs in there */
	    } /* end if */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (umask_init) */

int umasker::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        void_f	b = umask_atforkbefore ;
	        void_f	a = umask_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
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
/* end method (umasker::fini) */

int umasker::get() noex {
	int		rs ;
	int		rs1 ;
	int		cmask = 0 ;
	    if ((rs = init()) >= 0) {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = mx.lockbegin) >= 0) { /* single */
			{
			    cmask = umask(0) ; /* in case of race! */
			    umask(cmask) ;
			}
	                rs1 = mx.lockend ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	            rs1 = uc_forklockend() ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (forklock) */
	    } /* end if (init) */
	cmask &= INT_MAX ;
	return (rs >= 0) ? cmask : rs ;
}
/* end method (umasker::get) */

int umasker::setmode(mode_t cmask) noex {
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	    if ((rs = init()) >= 0) {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = mx.lockbegin) >= 0) { /* single */
			{
			    omask = umask(cmask) ;
			}
	                rs1 = mx.lockend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	            rs1 = uc_forklockend() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (forklock) */
	    } /* end if (init) */
	omask &= INT_MAX ;
	return (rs >= 0) ? omask : rs ;
}
/* end subroutine (umasker::set) */

static void umask_atforkbefore() noex {
	umask_data.forkbefore() ;
}
/* end subroutine (umask_atforkbefore) */

static void umask_atforkafter() noex {
	umask_data.forkafter() ;
}
/* end subroutine (umask_atforkafter) */

static void umask_exit() noex {
	umask_data.fini() ;
}
/* end subroutine (umask_exit) */


