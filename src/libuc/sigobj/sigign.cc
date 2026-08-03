/* sigign SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage process signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This was created along with the DATE object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sigign

	Description:
	This small object provides a way to manage (block, ignore,
	and catch) process signals.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsig.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"sigign.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;


/* local typedefs */


/* external subroutines */


/* exported variables */


/* local structures */


/* forward references */

local int	getnhandles(cint *) noex ;


/* local variables */

constexpr int	sigouts[] = {
	SIGTTOU,
	0
} ; /* end array (sigouts) */


/* exported variables */


/* exported subroutines */

int sigign_start(sigign *iap,cint *ignores) noex {
    	SIGIGN		*hop = iap ;
	int		rs = SR_FAULT ;
	int		nhandles = 0 ;
	if (iap) ylikely {
	    if (ignores == nullptr) {
		ignores = sigouts ;
	    }
	    rs = memclear(hop) ;
	    if (ignores != nullptr) {
	        if ((nhandles = getnhandles(ignores)) > 0) {
	            cint	sz = (nhandles * szof(sigign_ha)) ;
	            iap->nhandles = nhandles ;
	            if (void *vp{} ; (rs = libmem.mall(sz,&vp)) >= 0) {
	                iap->handles = (sigign_ha *) vp ;
	                if (sigset_t nsm{} ; (rs = uc_sigsetempty(&nsm)) >= 0) {
                            SIGACTION       san{} ;
                            SIGACTION       *sap ;
                            sigign_ha       *hp = iap->handles ;
                            int             hsig ;
                            int             i ; /* used-afterwards */
                            for (i = 0 ; ignores[i] != 0 ; i += 1) {
                                hsig = ignores[i] ;
                                hp[i].sig = hsig ;
                                sap = &hp[i].action ;
                                san.sa_handler = SIG_IGN ;
                                san.sa_mask = nsm ;
                                san.sa_flags = 0 ;
                                rs = u_sigaction(hsig,&san,sap) ;
                                if (rs < 0) break ;
                            } /* end for */
                            if (rs < 0) {
                                for (int j = (i - 1) ; j >= 0 ; j -= 1) {
                                    hsig = hp[j].sig ;
                                    sap = &hp[j].action ;
                                    u_sigaction(hsig,sap,nullptr) ;
                                }
                            } /* end if (error handling) */
			} /* end if (uc_sigsetempty) */
		        if (rs < 0) {
	      		    libmem.free(iap->handles) ;
			    iap->handles = nullptr ;
	 	        } /* end if (error handling) */
	            } /* end if (memory allocations) */
	        } /* end if (handles) */
	    } /* end if (ignores) */
	    if (rs >= 0) {
	        iap->magval = SIGIGN_MAGIC ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? nhandles : rs ;
} /* end subroutine (sigign_start) */

int sigign_finish(sigign *iap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (iap) ylikely {
	    rs = SR_NOTOPEN ;
	    if (iap->magval == SIGIGN_MAGIC) ylikely {
	        if (iap->handles) {
	            SIGACTION	*sap ;
	            for (int j = (iap->nhandles-1)  ; j >= 0 ; j -= 1) {
	                cint	hsig = iap->handles[j].sig ;
	                sap = &iap->handles[j].action ;
	                rs1 = u_sigaction(hsig,sap,nullptr) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end for */
	            rs1 = libmem.free(iap->handles) ;
	            if (rs >= 0) rs = rs1 ;
	            iap->handles = nullptr ;
	        } /* end if */
	        iap->magval = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sigign_finish) */


/* local subroutines */

local int getnhandles(cint *ignores) noex {
    	int		i = 0 ; /* used-afterwards */
	if (ignores) ylikely {
	    while (ignores[i] > 0) {
	        i += 1 ;
	    }
	} /* end if (non-null) */
	return i ;
} /* end subroutine (getnhandles) */

int sigign_co::operator () (cint *sigs) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = sigign_start(op,sigs) ;
	}
	return rs ;
} /* end method (sigign::operator) */

sigign_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case sigignmem_start:
	        rs = sigign_start(op,nullptr) ;
	        break ;
	    case sigignmem_finish:
	        rs = sigign_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (sigign::operator) */

void sigign::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("sigign",rs,"fini-finish") ;
	}
} /* end method (sigign::dtor) */


