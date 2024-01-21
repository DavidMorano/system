/* sysdbfname SUPPORT */
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
	int sysdbfnameget(int n,cc *fn,cc **rpp) noex

	Arguments:
	n		name to retrieve
	fn		given file-name (or NULL if want the default)
	rpp		pointer to the result c-string

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<csignal>
#include	<usystem.h>
#include	<usupport.h>
#include	<timewatch.hh>
#include	<ptm.h>

#include	"sysdbfnames.hh"
#include	"sysdbmgr.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sysdbmgr {
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
	~sysdbmgr() noex {
	    int		rs = fini() ;
	    if (rs < 0) {
		ulogerror("sysdbmgr",rs,"fini") ;
	    }
	} ; /* end dtor (sysdbmgr) */
    } ; /* end structure (sysdbmgr) */
}


/* forward references */

extern "C" {
    int		sysdbmgr_init() noex ;
    int		sysdbmgr_fini() noex ;
}

extern "C" {
    static void	sysdbmgr_atforkbefore() noex ;
    static void	sysdbmgr_atforkparent() noex ;
    static void	sysdbmgr_atforkchild() noex ;
    static void	sysdbmgr_exit() noex ;
}


/* lcoal variables */

static sysdbmgr			sysdbmgr_data ;


/* exported subroutines */

int uc_getpid(void) noex {
	int		rs ;
	if ((rs = sysdbmgr_data.igetpid()) == SR_NXIO) {
	    rs = sysdbmgr_data.pid ;
	}
	return rs ;
}

void uc_setpid(pid_t pid) noex {
	if (pid < 0) pid = getpid() ;
	sysdbmgr_data.pid = pid ;
}
/* end subroutine (uc_setpid) */

int sysdbmgr_init() noex {
	return sysdbmgr_data.init() ;
}

int sysdbmgr_fini() noex {
	return sysdbmgr_data.fini() ;
}


/* local subroutines */

int sysdbmgr::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = sysdbmgr_atforkbefore ;
	            void_f	ap = sysdbmgr_atforkparent ;
	            void_f	ac = sysdbmgr_atforkchild ;
	            if ((rs = uc_atfork(b,ap,ac)) >= 0) {
	                if ((rs = uc_atexit(sysdbmgr_exit)) >= 0) {
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
/* end method (sysdbmgr::init) */

int sysdbmgr::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        void_f	b = sysdbmgr_atforkbefore ;
	        void_f	ap = sysdbmgr_atforkparent ;
	        void_f	ac = sysdbmgr_atforkchild ;
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
/* end method (sysdbmgr::fini) */

int sysdbmgr::igetpid() noex {
	int		rs = SR_OK ;
	if (!finit) rs = init() ;
	if (rs >= 0) {
	    if (pid == 0) pid = getpid() ;
	    rs = pid ;
	}
	return rs ;
}
/* end subroutine (sysdbmgr::igetpid) */

static void sysdbmgr_atforkbefore() noex {
	sysdbmgr_data.atforkbefore() ;
}
/* end subroutine (sysdbmgr_atforkbefore) */

static void sysdbmgr_atforkparent() noex {
	sysdbmgr_data.atforkparent() ;
}
/* end subroutine (sysdbmgr_atforkparent) */

static void sysdbmgr_atforkchild() noex {
	sysdbmgr_data.atforkchild() ;
}
/* end subroutine (sysdbmgr_atforkchild) */

static void sysdbmgr_exit() noex {
	sysdbmgr_data.fini() ;
}
/* end subroutine (sysdbmgr_exit) */


