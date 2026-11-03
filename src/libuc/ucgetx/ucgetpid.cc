/* ucgetpid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get the current process PID (quickly and fork-safely) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
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
	A. We need to know when a |fork(2)| occurs so that the PID
	can be updated on the next read-access of it.  That feature
	comes with the associated baggage (below).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucatexit.h>		/* LIBUC */
#include	<ucforkat.h>		/* LIBUC */
#include	<timewatch.hh>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetpid.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		ucgetpid_init() noex ;
    int		ucgetpid_fini() noex ;
} /* end extern (C) */


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
        void forkatbefore() noex {
	    mx.lockbeg() ;
        } ; /* end */
        void forkatparent() noex {
	    mx.lockend() ;
        } ; /* end */
        void forkatchild() noex {
	    pid = 0 ;
	    mx.lockend() ;
        } ; /* end */
	destruct ucgetpid() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("ucgetpid",rs,"dtor-fini") ;
	    } /* end */
	} ; /* end dtor */
	int igetpid() noex ;
    } ; /* end structure (ucgetpid) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	ucgetpid_forkatbefore() noex ;
    local void	ucgetpid_forkatparent() noex ;
    local void	ucgetpid_forkatchild() noex ;
    local void	ucgetpid_exit() noex ;
} /* end extern (C) */


/* lcoal variables */

static ucgetpid		ucgetpid_data ;


/* exported variables */

libuc::ucpider		ucpid ;
libuc::ucsider		ucsid ;


/* exported subroutines */

int uc_getpid(void) noex {
	int		rs ;
	if ((rs = ucgetpid_data.igetpid()) == SR_NXIO) {
	    rs = ucgetpid_data.pid ;
	} /* end */
	return rs ;
} /* end subroutine */

int uc_setpid(pid_t pid) noex {
	if (pid < 0) pid = getpid() ;
	ucgetpid_data.pid = pid ;
	return SR_OK ;
} /* end subroutine (uc_setpid) */

int ucgetpid_init() noex {
	return ucgetpid_data.init() ;
} /* end */

int ucgetpid_fini() noex {
	return ucgetpid_data.fini() ;
} /* end */


/* local subroutines */

int ucgetpid::init() noex {
	int		rs = SR_NXIO ;
	int		fr = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = ucgetpid_forkatbefore ;
	            void_f	ap = ucgetpid_forkatparent ;
	            void_f	ac = ucgetpid_forkatchild ;
	            if ((rs = uc_forkatrec(b,ap,ac)) >= 0) {
			void_f	e = ucgetpid_exit ;
	                if ((rs = uc_atexit(e)) >= 0) {
	                    finitdone = true ;
	                    fr = true ;
	                } /* end */
	                if (rs < 0) {
	                    uc_forkatexp(b,ap,ac) ;
			} /* end if (error) */
	            } /* end if (uc_forkat) */
	 	    if (rs < 0) {
		        mx.destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		} /* end if (error) */
	    } else if (!finitdone) {
	        timewatch	tw(to) ;
	        cauto lamb = [this] () -> int {
	            int		rsl = SR_OK ; /* GCC 'shadow' complaint */
	            if (!finit) {
		        rsl = SR_LOCKFAIL ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            } /* end */
	            return rsl ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? fr : rs ;
} /* end method (ucgetpid::init) */

int ucgetpid::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        void_f	b = ucgetpid_forkatbefore ;
	        void_f	ap = ucgetpid_forkatparent ;
	        void_f	ac = ucgetpid_forkatchild ;
	        rs1 = uc_forkatexp(b,ap,ac) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    } /* end */
	    finitdone = false ;
	    finit = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end method (ucgetpid::fini) */

int ucgetpid::igetpid() noex {
	int		rs = SR_OK ;
	if (!finit) rs = init() ;
	if (rs >= 0) {
	    if (pid == 0) pid = getpid() ;
	    rs = conv<int>(pid) ;
	} /* end */
	return rs ;
} /* end subroutine (ucgetpid::igetpid) */

local void ucgetpid_forkatbefore() noex {
	ucgetpid_data.forkatbefore() ;
} /* end subroutine (ucgetpid_forkatbefore) */

local void ucgetpid_forkatparent() noex {
	ucgetpid_data.forkatparent() ;
} /* end subroutine (ucgetpid_forkatparent) */

local void ucgetpid_forkatchild() noex {
	ucgetpid_data.forkatchild() ;
} /* end subroutine (ucgetpid_forkatchild) */

local void ucgetpid_exit() noex {
	ucgetpid_data.fini() ;
} /* end subroutine (ucgetpid_exit) */

namespace libuc {
    ucpider::operator int () noex {
    	return uc_getpid() ;
    } /* end method (ucpider::operator) */
    ucsider::operator int () noex {
    	return u_getsid(0) ;
    } /* end method (ucpider::operator) */
} /* end namespace (libuc) */


