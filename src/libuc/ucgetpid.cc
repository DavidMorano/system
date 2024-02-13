/* ucgetpid SUPPORT */
/* lang=C++20 */

/* get the current process PID (quickly and fork-safely) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D-­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getpid

	Description:
	We get (and possibly set) our PID.

	Symopsis:
	int uc_getpid(void) noex

	Arguments:
	-

	Returns:
	>=0		the current process PID
	<0		error (system-return)

	Notes:
	Q. Why all of the fuss?
	A. We need to know when a |fork(2)| occurs so that the PID can
	be updated on the next read-access of it. That feature comes with
	the associated baggage (below).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<csignal>
#include	<usystem.h>
#include	<usupport.h>
#include	<timewatch.hh>
#include	<ptm.h>

#include	"ucgetpid.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		ucgetpid_init() noex ;
    int		ucgetpid_fini() noex ;
}


/* external variables */


/* local structures */

namespace {
    struct ucgetpid {
	ptm		mx ;		/* data mutex */
	pid_t		pid ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init() noex ;
	int fini() noex ;
	int igetpid() noex ;
        void atforkbefore() noex {
	    mx.lockbegin() ;
        }
        void atforkparent() noex {
	    mx.lockend() ;
        }
        void atforkchild() noex {
	    pid = 0 ;
	    mx.lockend() ;
        }
	~ucgetpid() noex {
	    cint	rs = fini() ;
	    if (rs < 0) {
		ulogerror("ucgetpid",rs,"dtor-fini") ;
	    }
	} ; /* end dtor */
    } ; /* end structure (ucgetpid) */
}


/* forward references */

extern "C" {
    static void	ucgetpid_atforkbefore() noex ;
    static void	ucgetpid_atforkparent() noex ;
    static void	ucgetpid_atforkchild() noex ;
    static void	ucgetpid_exit() noex ;
}


/* lcoal variables */

static ucgetpid			ucgetpid_data ;


/* exported variables */


/* exported subroutines */

int uc_getpid(void) noex {
	int		rs ;
	if ((rs = ucgetpid_data.igetpid()) == SR_NXIO) {
	    rs = ucgetpid_data.pid ;
	}
	return rs ;
}

void uc_setpid(pid_t pid) noex {
	if (pid < 0) pid = getpid() ;
	ucgetpid_data.pid = pid ;
}
/* end subroutine (uc_setpid) */

int ucgetpid_init() noex {
	return ucgetpid_data.init() ;
}

int ucgetpid_fini() noex {
	return ucgetpid_data.fini() ;
}


/* local subroutines */

int ucgetpid::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = ucgetpid_atforkbefore ;
	            void_f	ap = ucgetpid_atforkparent ;
	            void_f	ac = ucgetpid_atforkchild ;
	            if ((rs = uc_atforkrecord(b,ap,ac)) >= 0) {
	                if ((rs = uc_atexit(ucgetpid_exit)) >= 0) {
	                    finitdone = true ;
	                    f = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexpunge(b,ap,ac) ;
			}
	            } /* end if (uc_atfork) */
	 	    if (rs < 0) {
		        mx.destroy() ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (!finitdone) {
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
/* end method (ucgetpid::init) */

int ucgetpid::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        void_f	b = ucgetpid_atforkbefore ;
	        void_f	ap = ucgetpid_atforkparent ;
	        void_f	ac = ucgetpid_atforkchild ;
	        rs1 = uc_atforkexpunge(b,ap,ac) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finitdone = false ;
	    finit = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end method (ucgetpid::fini) */

int ucgetpid::igetpid() noex {
	int		rs = SR_OK ;
	if (!finit) rs = init() ;
	if (rs >= 0) {
	    if (pid == 0) pid = getpid() ;
	    rs = pid ;
	}
	return rs ;
}
/* end subroutine (ucgetpid::igetpid) */

static void ucgetpid_atforkbefore() noex {
	ucgetpid_data.atforkbefore() ;
}
/* end subroutine (ucgetpid_atforkbefore) */

static void ucgetpid_atforkparent() noex {
	ucgetpid_data.atforkparent() ;
}
/* end subroutine (ucgetpid_atforkparent) */

static void ucgetpid_atforkchild() noex {
	ucgetpid_data.atforkchild() ;
}
/* end subroutine (ucgetpid_atforkchild) */

static void ucgetpid_exit() noex {
	ucgetpid_data.fini() ;
}
/* end subroutine (ucgetpid_exit) */


