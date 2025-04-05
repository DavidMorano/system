/* ucgetus SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usysflag.h>
#include	<timewatch.hh>
#include	<sncpyx.h>
#include	<ptm.h>
#include	<localmisc.h>

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
        }
        void atforkafter() noex {
	    mx.lockend() ;
        }
	~ucgetus() noex {
	    if (cint rs = fini() ; rs < 0) {
		ulogerror("ucgetus",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (ucgetus) */
    } ; /* end struct (ucgetus) */
}


/* forward references */

extern "C" {
    static void		ucgetus_atforkbefore() noex ;
    static void		ucgetus_atforkafter() noex ;
    static void		ucgetus_exit() noex ;
}


/* local variables */

static ucgetus		ucgetus_data ;


/* exported variables */


/* exported subroutines */

int uc_getusbegin() noex {
	return ucgetus_data.getusbegin() ;
}

int uc_getusend() noex {
	return ucgetus_data.getusend() ;
}

int uc_getusent(char *rbuf,int rlen) noex {
	return ucgetus_data.getusent(rbuf,rlen) ;
}


/* local subroutines */

int ucgetus::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    if (!finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = ucgetus_atforkbefore ;
	            void_f	a = ucgetus_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(ucgetus_exit)) >= 0) {
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
	        } /* end if (ptm-create) */
	        if (rs < 0) finit = false ;
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rsl = SR_OK ; /* GCC 'shadow' complaint */
	            if (!finit) {
		        rsl = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        rsl = 1 ;			/* <- OK ready */
	            }
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucgetus::init) */

int ucgetus::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (!fvoid.testandset)) {
	    if (factive) {
		factive = false ;
		endusershell() ;
	    }
	    {
	        void_f	b = ucgetus_atforkbefore ;
	        void_f	a = ucgetus_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
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
}
/* end subroutine (ucgetus::fini) */

int ucgetus::getusbegin() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = init()) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	 	if ((rs = mx.lockbegin) >= 0) { /* single */
		    {
			errno = 0 ;
		        factive = true ;
		        setusershell() ;
			rs = (- errno) ;
		    }
		    rs1 = mx.lockend ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mutex) */
	        rs1 = uc_forklockend() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return rs ;
}
/* end subroutine (ucgetus::getusbegin) */

int ucgetus::getusend() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = init()) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	 	if ((rs = mx.lockbegin) >= 0) { /* single */
		    {
			errno = 0 ;
		        factive = false ;
		        endusershell() ;
			rs = (- errno) ;
		    }
		    rs1 = mx.lockend ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mutex) */
	        rs1 = uc_forklockend() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return rs ;
}
/* end subroutine (ucgetus::getusend) */

int ucgetus::getusent(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_OVERFLOW ;
	    if (rlen > 0) {
	        if ((rs = init()) >= 0) {
	            if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	 	        if ((rs = mx.lockbegin) >= 0) { /* single */
		            cchar	*rp ;
		            factive = true ;
		            errno = 0 ;
		            if ((rp = (cchar *) getusershell()) != nullptr) {
	    	                rs = sncpy1(rbuf,rlen,rp) ;
			        len = rs ;
		            } else { /* this is really extra safety */
			        if (errno != 0) rs = (- errno) ;
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
}
/* end subroutine (ucgetus::getusent) */

static void ucgetus_atforkbefore() noex {
	ucgetus_data.atforkbefore() ;
}
/* end subroutine (ucgetus_atforkbefore) */

static void ucgetus_atforkafter() noex {
	ucgetus_data.atforkafter() ;
}
/* end subroutine (ucgetus_atforkafter) */

static void ucgetus_exit() noex {
	int	rs = ucgetus_data.fini() ;
	if (rs < 0) {
	    ulogerror("ucgetus",rs,"exit-fini") ;
	}
}
/* end subroutine (ucgetus_exit) */


