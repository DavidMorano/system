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
#include	<sys/types.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucsig.h>
#include	<localmisc.h>

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

static int	getnhandles(cint *) noex ;


/* local variables */

constexpr int	sigouts[] = {
	SIGTTOU,
	0
} ;


/* exported variables */


/* exported subroutines */

int sigign_start(sigign *iap,cint *ignores) noex {
    	SIGIGN		*hop = iap ;
	int		rs = SR_FAULT ;
	int		nhandles = 0 ;
	if (iap) {
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
	        iap->magic = SIGIGN_MAGIC ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? nhandles : rs ;
}
/* end subroutine (sigign_start) */

int sigign_finish(sigign *iap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (iap) {
	    rs = SR_NOTOPEN ;
	    if (iap->magic == SIGIGN_MAGIC) {
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
	        iap->magic = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigign_finish) */


/* local subroutines */

static int getnhandles(cint *ignores) noex {
    	int		i = 0 ; /* used-afterwards */
	if (ignores) {
	    while (ignores[i] > 0) {
	        i += 1 ;
	    }
	} /* end if (non-null) */
	return i ;
}
/* end subroutine (getnhandles) */


