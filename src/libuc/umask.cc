/* umask SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® UMASK (file-creation-mask) management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	umask

	Description:
	This module serves to provide two functions that manipulate
	the UNIX® UMASK.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* POSIX® |mode_t| */
#include	<csignal>		/* CSTD |sig_atomic_t| */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<aflag.hh>		/* LIBU */
#include	<timewatch.hh>		/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucfork.h>		/* LIBUC */
#include	<ucatfork.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"umask.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* exported variables */


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
	destruct umasker() {
            if (cint rs = fini() ; rs < 0) {
                ulogerror("umask",rs,"dtor-fini") ;
            }
	} ;
    } ; /* end struct (umasker) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	umask_atforkbefore() noex ;
    local void	umask_atforkafter() noex ;
    local void	umask_exit() noex ;
}


/* local variables */

static umasker	umask_data ;


/* exported variables */


/* exported subroutines */

int umask_init() noex {
	return umask_data.init() ;
} /* end subroutine (umask_init) */

int umask_fini() noex {
	return umask_data.fini() ;
} /* end subroutine (umask_fini) */

int umaskget() noex {
	int		rs ;
	int		rs1 ;
	int		cmask = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    {
		rs = umask_data.get() ;
		cmask = rs ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? cmask : rs ;
} /* end subroutine (umaskget) */

int umaskset(mode_t cmask) noex {
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    {
		rs = umask_data.setmode(cmask) ;
		omask = rs ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? omask : rs ;
} /* end subroutine (umaskset) */


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
	            if ((rs = uc_atforkrec(b,a,a)) >= 0) {
			void_f	e = umask_exit ;
	                if ((rs = uc_atexit(e)) >= 0) {
	    	            finitdone = true ;
		            f = true ;
		        }
		        if (rs < 0) {
		            uc_atforkexp(b,a,a) ;
			} /* end if (error) */
	            } /* end if (uc_atfork) */
	 	    if (rs < 0) {
		        mx.destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		} /* end if (error) */
            } else if (! finitdone) { 
                timewatch       tw(to) ;
                cauto lamb = [this] () -> int {
                    int         rsl = SR_OK ;
                    if (!finit) {
                        rsl = SR_LOCKFAIL ;              /* <- failure */
                    } else if (finitdone) {
                        rsl = 1 ;                        /* <- OK ready */
                    }                       
                    return rsl ;
                } ; /* end lambda (lamb) */ 
                rs = tw(lamb) ;         /* <- time-watching occurs in there */
	    } /* end if */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (umask_init) */

int umasker::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        void_f	b = umask_atforkbefore ;
	        void_f	a = umask_atforkafter ;
	        rs1 = uc_atforkexp(b,a,a) ;
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
} /* end method (umasker::fini) */

int umasker::get() noex {
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	if ((rs = init()) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	        if ((rs = mx.lockbegin) >= 0) { /* single */
		    {
			mode_t cm = umask(0) ; /* in case of race! */
			umask(cm) ;
			omask = int(cm) ;
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
} /* end method (umasker::get) */

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
} /* end subroutine (umasker::set) */

local void umask_atforkbefore() noex {
	umask_data.forkbefore() ;
} /* end subroutine (umask_atforkbefore) */

local void umask_atforkafter() noex {
	umask_data.forkafter() ;
} /* end subroutine (umask_atforkafter) */

local void umask_exit() noex {
	umask_data.fini() ;
} /* end subroutine (umask_exit) */


