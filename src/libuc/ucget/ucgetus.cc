/* ucgetus SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucgetus

	Description:
	These subroutines manage the retrieval of system-configured
	user-shells.  These subroutines are multi-thread safe, but
	if multiple threads use them simultaneously, each thread
	will enumerate a separate set of the database (as eeems to
	be the standard as decreed by the UNIX® gods).

	Notes:
	There is no header file that declares the underlying UNIX®
	subroutines!  I guess they(?) forgot to make one up!

	Q. Do these subroutines need to be multi-thread safe?
	A. What do you think?

	Q. Where is the data we are protecting with our mutex lock?
	A. It is the private static data that is located inside the
	   |getusershell(3c)| subroutine.

	Q. All of this (locking) just to protect the private static
	   data inside of the |getusershell(3c)| subroutine?
	A. Yes. Pretty much.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<ucatexit.h>		/* |uc_atexit(3uc)| */
#include	<ucatfork.h>		/* |uc_atfork{x}(3uc)| */
#include	<ucfork.h>		/* |uc_forklock{x}(3uc)| */
#include	<timewatch.hh>
#include	<sncpyx.h>
#include	<localmisc.h>		/* LIBU */

#include	"ucgetus.h"
#include	"ucgetxx.hh"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ucgetus {
	ptm		mx ;		/* data mutex */
	aflag		factive ;	/* "set" or "get" was done */
	aflag		finit ;
	aflag		finitdone ;
	aflag		fvoid ;
	int init() noex ;
	int fini() noex ;
	int getusbegin() noex ;
	int getusend() noex ;
	int getusent(char *,int) noex ;
        void atforkbefore() noex {
	    mx.lockbegin() ;
        } ;
        void atforkafter() noex {
	    mx.lockend() ;
        } ;
	destruct ucgetus() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("ucgetus",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (ucgetus) */
    } ; /* end struct (ucgetus) */
} /* end namespace */


/* forward references */

extern "C" {
    local void		ucgetus_atforkbefore() noex ;
    local void		ucgetus_atforkafter() noex ;
    local void		ucgetus_exit() noex ;
} /* end extern (C) */


/* local variables */

static ucgetus		ucgetus_data ;


/* exported variables */


/* exported subroutines */

int uc_getusbegin() noex {
	return ucgetus_data.getusbegin() ;
} /* end subroutine */

int uc_getusend() noex {
	return ucgetus_data.getusend() ;
} /* end subroutine */

int uc_getusent(char *rbuf,int rlen) noex {
	return ucgetus_data.getusent(rbuf,rlen) ;
} /* end subroutine */


/* local subroutines */

int ucgetus::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) ylikely {
	            void_f	b = ucgetus_atforkbefore ;
	            void_f	a = ucgetus_atforkafter ;
	            if ((rs = uc_atforkrec(b,a,a)) >= 0) ylikely {
	                if ((rs = uc_atexit(ucgetus_exit)) >= 0) ylikely {
	                    finitdone = true ;
	                    f = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexp(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
	 	    if (rs < 0) {
		        mx.destroy() ;
		    }
	        } /* end if (ptm-create) */
	        if (rs < 0) finit = false ;
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rsl = SR_OK ; /* GCC 'shadow' complaint */
	            if (!finit) {
		        rsl = SR_LOCKFAIL ;		/* <- failure */
	            } else if (finitdone) {
		        rsl = 1 ;			/* <- OK ready */
	            }
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (ucgetus::init) */

int ucgetus::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    if (factive) {
		factive = false ;
		endusershell() ;
	    }
	    {
	        void_f	b = ucgetus_atforkbefore ;
	        void_f	a = ucgetus_atforkafter ;
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end subroutine (ucgetus::fini) */

int ucgetus::getusbegin() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = init()) >= 0) ylikely {
	    if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	 	if ((rs = mx.lockbegin) >= 0) { /* single */
		    {
			errno = 0 ;
		        factive = true ;
		        setusershell() ;
			rs = (neg errno) ;
		    }
		    rs1 = mx.lockend ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mutex) */
	        rs1 = uc_forklockend() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return rs ;
} /* end subroutine (ucgetus::getusbegin) */

int ucgetus::getusend() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = init()) >= 0) ylikely {
	    if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	 	if ((rs = mx.lockbegin) >= 0) { /* single */
		    {
			errno = 0 ;
		        factive = false ;
		        endusershell() ;
			rs = (neg errno) ;
		    }
		    rs1 = mx.lockend ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mutex) */
	        rs1 = uc_forklockend() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return rs ;
} /* end subroutine (ucgetus::getusend) */

int ucgetus::getusent(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (rlen > 0) ylikely {
	        if ((rs = init()) >= 0) ylikely {
	            if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	 	        if ((rs = mx.lockbegin) >= 0) { /* single */
		            cchar	*rp ;
		            factive = true ;
		            errno = 0 ;
		            if ((rp = getusershell()) != nullptr) {
	    	                rs = sncpy1(rbuf,rlen,rp) ;
			        len = rs ;
		            } else { /* this is really extra safety */
			        if (errno != 0) rs = (neg errno) ;
		            }
		            rs1 = mx.lockend ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (mutex) */
	                rs1 = uc_forklockend() ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (forklock) */
	        } /* end if (initialized) */
	    } /* end if (non-negative buffer length) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (ucgetus::getusent) */

local void ucgetus_atforkbefore() noex {
	ucgetus_data.atforkbefore() ;
} /* end subroutine (ucgetus_atforkbefore) */

local void ucgetus_atforkafter() noex {
	ucgetus_data.atforkafter() ;
} /* end subroutine (ucgetus_atforkafter) */

local void ucgetus_exit() noex {
	if (cint rs = ucgetus_data.fini() ; rs < 0) {
	    ulogerror("ucgetus",rs,"exit-fini") ;
	}
} /* end subroutine (ucgetus_exit) */


