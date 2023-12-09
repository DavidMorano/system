/* ucfork */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* special handling for |fork(2)| */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Mame:
	uc_fork() noex
	uc_forklockbegin(int) noex
	uc_forklockend() noex

	Description:
	These are subroutines implement a feature where there is a
	read-write lock protecting the call to the system |fork(2)|
	subroutine. This is useful for linrary-based user-level
	emulated kernel calls that have statically allocated
	mutex-locks within them.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<signal.h>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<timewatch.hh>
#include	<sigblocker.h>
#include	<lockrw.h>
#include	<ucgetpid.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */

namespace {
    struct ucfork {
	lockrw		lock ;		/* critical-section lock */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init() noex ;
	int fini() noex ;
	int stdfork() noex ;
	int lockbegin(int) noex ;
	int lockend() noex ;
	~ucfork() noex {
	    int		rs = fini() ;
	    if (rs < 0) {
		ulogerror("ucfork",rs,"fini") ;
	    }
	} ; /* end dtor (ucfork) */
    } ; /* end struct (ucfork) */
}


/* forward references */

extern "C" {
    int		ucfork_init() noex ;
    int		ucfork_fini() noex ;
}

extern "C" {
    static void	ucfork_exit() noex ;
}



/* local variables */

static ucfork			ucfork_data ;


/* exported subroutines */

int ucfork_init() noex {
	return ucfork_data.init() ;
}

int ucfork_fini() noex {
	return ucfork_data.fini() ;
}

int uc_fork() noex {
	return ucfork_data.stdfork() ;
}

int uc_forklockbegin(int to) noex {
	return ucfork_data.lockbegin(to) ;
}

int uc_forklockend() noex {
	return ucfork_data.lockend() ;
}


/* local subroutines */

int ucfork::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    if (! finit.testandset) {
	        if ((rs = lock.create) >= 0) {
	            if ((rs = uc_atexit(ucfork_exit)) >= 0) {
	                f = true ;
	                bool(finitdone.set) ;
	            }
	            if (rs < 0) {
	                lock.destroy() ;
	            }
	        } /* end if (lockrw-create) */
	        if (rs < 0)
	            bool(finit.clear) ;
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;
	            } else if (finitdone) {
		        rs = 1 ;
	            }
	            return rs ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (ucfork::init) */

int ucfork::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone) {
	    fvoid.set() ;
	    finitdone.clear() ;
	    {
	        rs1 = lock.destroy() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit.clear() ;
	    finitdone.clear() ;
	} /* end if (atexit registered) */
	return rs ;
}
/* end method (ucfork::fini) */

int ucfork::stdfork() noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		pid = 0 ;
	if ((rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	        if ((rs = lock.wrlock) >= 0) {
	            rs = u_fork() ;
		    pid = rs ;
	            if (rs == 0) { /* child */
	                fini() ;
			uc_setpid(0) ;
	                if ((rs = init()) > 0) {
	                    rs = 0 ;
	                }
	            } else if (rs > 0) {
	                lock.unlock() ;
	            }
	        } /* end if (lockrw) */
	    } /* end if (ucfork_init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? pid : rs ;
}
/* end mrthod (ucfork::stdfork) */

int ucfork::lockbegin(int to) noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = b.start) >= 0) {
	    if ((rs = init()) >= 0) {
	        rs = lock.rdlock(to) ;
		rc = rs ;
	    } /* end if (ucfork_init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (ucfork::lockbegin) */

int ucfork::lockend() noex {
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = b.start) >= 0) {
	    if (finit) {
	        rs = lock.unlock ;
		rc = rs ;
	    } else {
	        rs = SR_NOANODE ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (ucfork::lockend) */

static void ucfork_exit() noex {
	ucfork_data.fvoid = true ;
}


