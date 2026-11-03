/* ucfilemask SUPPORT */
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
	uc_filemaskget
	uc_filemaskset

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
#include	<ucforkat.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucfilemask.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* exported variables */


/* external subroutines */


/* local structures */

namespace {
    struct ucfilemasker {
	ptm		mx ;		/* data mutex */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init	() noex ;
	int fini	() noex ;
	int get		() noex ;
	int setmode	(mode_t) noex ;
	void forkbefore() noex {
	    mx.lockbeg() ;
	} ; /* end */
	void forkafter() noex {
	    mx.lockend() ;
	} ; /* end */
	destruct ucfilemasker() {
            if (cint rs = fini() ; rs < 0) {
                ulogerror("ucfilemask",rs,"dtor-fini") ;
            }
	} ; /* end destruct */
    } ; /* end struct (ucfilemasker) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	ucfilemask_forkatbefore() noex ;
    local void	ucfilemask_forkatafter() noex ;
    local void	ucfilemask_exit() noex ;
} /* end extern (C) */


/* local variables */

static ucfilemasker	ucfilemask_data ;


/* exported variables */


/* exported subroutines */

int ucfilemask_init() noex {
	return ucfilemask_data.init() ;
} /* end subroutine (ucfilemask_init) */

int ucfilemask_fini() noex {
	return ucfilemask_data.fini() ;
} /* end subroutine (ucfilemask_fini) */

int uc_filemaskget() noex {
	int		rs ;
	int		rs1 ;
	int		cmask = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	    {
		rs = ucfilemask_data.get() ;
		cmask = rs ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? cmask : rs ;
} /* end subroutine (uc_filemaskget) */

int uc_filemaskset(mode_t cmask) noex {
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	    {
		rs = ucfilemask_data.setmode(cmask) ;
		omask = rs ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? omask : rs ;
} /* end subroutine (uc_filemaskset) */


/* local subroutines */

int ucfilemasker::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) ylikely {
	            void_f	b = ucfilemask_forkatbefore ;
	            void_f	a = ucfilemask_forkatafter ;
	            if ((rs = uc_forkatrec(b,a,a)) >= 0) ylikely {
			void_f	e = ucfilemask_exit ;
	                if ((rs = uc_atexit(e)) >= 0) ylikely {
	    	            finitdone = true ;
		            f = true ;
		        } /* end */
		        if (rs < 0) {
		            uc_forkatexp(b,a,a) ;
			} /* end if (error) */
	            } /* end if (uc_forkat) */
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
                    } /* end */
                    return rsl ;
                } ; /* end lambda (lamb) */ 
                rs = tw(lamb) ;         /* <- time-watching occurs in there */
	    } /* end if */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucfilemask_init) */

int ucfilemasker::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        void_f	b = ucfilemask_forkatbefore ;
	        void_f	a = ucfilemask_forkatafter ;
	        rs1 = uc_forkatexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    finit = false ;
	    finitdone = false ;
	} /* end if (atexit registered) */
	return rs ;
} /* end method (ucfilemasker::fini) */

int ucfilemasker::get() noex {
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	if ((rs = init()) >= 0) ylikely {
	    if ((rs = uc_forklockbeg(-1)) >= 0) ylikely { /* multi */
	        if ((rs = mx.lockbeg) >= 0) ylikely { /* single */
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
} /* end method (ucfilemasker::get) */

int ucfilemasker::setmode(mode_t cmask) noex {
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	if ((rs = init()) >= 0) ylikely {
	    if ((rs = uc_forklockbeg(-1)) >= 0) ylikely { /* multi */
	        if ((rs = mx.lockbeg) >= 0) ylikely { /* single */
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
} /* end subroutine (ucfilemasker::set) */

local void ucfilemask_forkatbefore() noex {
	ucfilemask_data.forkbefore() ;
} /* end subroutine (ucfilemask_forkatbefore) */

local void ucfilemask_forkatafter() noex {
	ucfilemask_data.forkafter() ;
} /* end subroutine (ucfilemask_forkatafter) */

local void ucfilemask_exit() noex {
	ucfilemask_data.fini() ;
} /* end subroutine (ucfilemask_exit) */


