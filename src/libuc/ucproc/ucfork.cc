/* ucfork SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* special handling for |fork(2)| */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */

/* revision history:

	= 1998-04-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Mame:
	uc_fork() noex
	uc_forklockbeg(int) noex
	uc_forklockend() noex

	Description:
	These are subroutines implement a feature where there is a
	read-write lock protecting the call to the system |fork(2)|
	subroutine. This is useful for linrary-based user-level
	emulated kernel calls that have statically allocated
	mutex-locks within them.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<timewatch.hh>		/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<ucatexit.h>		/* LIBUC */
#include	<ucgetpid.h>		/* LIBUC */
#include	<lockrw.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"ucfork.h"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		ucfork_init() noex ;
    int		ucfork_fini() noex ;
} /* end extern (C) */


/* external variables */


/* local structures */

namespace {
    struct forker_fl {
	uint		lock:1 ;
    } ; /* end struct (forker_fl) */
    struct forker {
	lockrw		lock ;		/* critical-section lock */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	forler_fl	fl ;
	int init	() noex ;
	int fini	() noex ;
	int sysfork	() noex ;
	int lockbeg	(int) noex ;
	int lockend	() noex ;
	destruct forker	() noex ;
    } ; /* end struct (forker) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	ucfork_exit() noex ;
} /* end extern (C) */


/* local variables */

static forker		ucfork_data ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int ucfork_init() noex {
	return ucfork_data.init() ;
} /* end subroutine */

int ucfork_fini() noex {
	return ucfork_data.fini() ;
} /* end subroutine */

int uc_fork() noex {
	return ucfork_data.sysfork() ;
} /* end subroutine */

int uc_forklockbeg(int to) noex {
	return ucfork_data.lockbeg(to) ;
} /* end subroutine */

int uc_forklockend() noex {
	return ucfork_data.lockend() ;
} /* end subroutine */


/* local subroutines */

int forker::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	DPRINTF("ent fvoid=%u\n",uint(fvoid)) ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = lock.create) >= 0) {
		    fl.lock = true ;
	            if ((rs = uc_atexit(ucfork_exit)) >= 0) {
	                f = true ;
	                finitdone.set() ;
	            } /* end */
	            if (rs < 0) {
	                lock.destroy() ;
	            } /* end if (error) */
	        } /* end if (lockrw-create) */
	        if (rs < 0) {
	            finit.clear() ;
		} /* end if (error) */
	    } else if (! finitdone) {
	        timewatch tw(to) ;
	        cauto lamb = [this] () -> int {
	            int rsl = SR_OK ;
	            if (! finit) {
		        rsl = SR_LOCKFAIL ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            } /* end */
	            return rsl ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	DPRINTF("ret rs=%d f=%u\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end method (forker::init) */

int forker::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    fvoid.set() ;
	    finitdone.clear() ;
	    if (fl.lock) {
		fl.lock = false ;
	        rs1 = lock.destroy() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    finit.clear() ;
	} /* end if (atexit registered) */
	return rs ;
} /* end method (forker::fini) */

int forker::sysfork() noex {
	int		rs ;
	int		rs1 ;
	int		pid = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	        if ((rs = lock.lockwr) >= 0) {
	            if ((rs = u_fork()) == 0) { /* child */
	                fini() ;
			uc_setpid(0) ;
	                if ((rs = init()) > 0) {
	                    rs = 0 ;
	                }
	            } else if (rs > 0) {
			pid = rs ;
	                lock.lockun() ;
	            } /* end */
	        } /* end if (lockrw) */
	    } /* end if (ucfork_init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	DPRINTF("ret rs=%d pid=%d\n",rs,pid) ;
	return (rs >= 0) ? pid : rs ;
} /* end mrthod (forker::sysfork) */

int forker::lockbeg(int to) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	        rs = lock.lockrd(to) ;
		rv = rs ;
	    } /* end if (ucfork_init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	DPRINTF("ret rs=%d rv=%u\n",rs,rv) ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (forker::lockbeg) */

int forker::lockend() noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    if (finit) {
	        rs = lock.lockun ;
		rv = rs ;
	    } else {
	        rs = SR_NOANODE ;
	    } /* end */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	DPRINTF("ret rs=%d rv=%u\n",rs,rv) ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (forker::lockend) */

forker::destruct forker() noex {
	if (cint rs = fini() ; rs < 0) {
	    ulogerror("ucfork",rs,"dtor-fini") ;
	} /* end */
} /* end dtor (forker::destruct) */

local void ucfork_exit() noex {
	ucfork_data.fvoid = true ;
} /* end subroutine */


