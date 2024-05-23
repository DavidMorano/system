/* sigman SUPPORT */
/* lang=C++20 */

/* manage process signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sigman

	Description:
	This small object provides a way to manage (block, ignore,
	and catch) process signals.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"sigman.h"


/* local defines */


/* imported namespaces */

typedef	sigman_hand *	handp ;


/* local typedefs */


/* external subroutines */


/* exported variables */


/* local subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sigman_start(sigman *iap,cint *blocks,cint *ignores,cint *catches,
		sigmanhand_f handle) noex {
	sigset_t	nsm ;
	int		rs = SR_OK ;
	int		i ;
	int		nhandles = 0 ;
	int		sz ;
	void		*p ;

	if (iap == nullptr) return SR_FAULT ;

	memclear(iap) ;

	if (handle == nullptr) {
	    handle = reinterpret_cast<sigmanhand_f>(SIG_IGN) ;
	}

/* blocks */

	if ((rs >= 0) && (blocks != nullptr)) {
	    uc_sigsetempty(&nsm) ;
	    for (i = 0 ; blocks[i] != 0 ; i += 1) {
	        uc_sigsetadd(&nsm,blocks[i]) ;
	    }
	    iap->nblocks = i ;
	    pthread_sigmask(SIG_BLOCK,&nsm,&iap->osm) ;
	} /* end if (blocks) */

/* calculate the allocations size */

	if (ignores != nullptr) {
	    for (i = 0 ; ignores[i] != 0 ; i += 1) {
	        nhandles += 1 ;
	    }
	}
	if (catches != nullptr) {
	    for (i = 0 ; catches[i] != 0 ; i += 1) {
	        nhandles += 1 ;
	    }
	}

	sz = (nhandles * sizeof(sigman_hand)) ;
	if ((rs >= 0) && (nhandles > 0) && ((rs = uc_malloc(sz,&p)) >= 0)) {
	    sigman_hand		*hp = handp(p) ;
	    SIGACTION		san{} ;
	    SIGACTION		*sap ;
	    int			hsig ;
	    int			j = 0 ;
	    iap->handles = handp(p) ;
	    iap->nhandles = nhandles ;
/* ignore these signals */
	    uc_sigsetempty(&nsm) ;
	    if (ignores != nullptr) {
	        for (i = 0 ; ignores[i] != 0 ; i += 1) {
	            hsig = ignores[i] ;
	            hp[j].sig = hsig ;
	            sap = &hp[j].action ;
	            san.sa_handler = SIG_IGN ;
	            san.sa_mask = nsm ;
	            san.sa_flags = 0 ;
	            rs = u_sigaction(hsig,&san,sap) ;
	            if (rs < 0) break ;
	            j += 1 ;
	        } /* end for */
	    } /* end if (ignores) */

/* catch (interrupt on) these signals */

	    if ((rs >= 0) && (catches != nullptr)) {
	        for (i = 0 ; catches[i] != 0 ; i += 1) {
	            hsig = catches[i] ;
	            hp[j].sig = hsig ;
	            sap = &hp[j].action ;
	            memset(&san,0,sizeof(SIGACTION)) ;
	            san.sa_handler = handle ;
	            san.sa_mask = nsm ;
	            san.sa_flags = 0 ;
	            rs = u_sigaction(hsig,&san,sap) ;
	            if (rs < 0) break ;
	            j += 1 ;
	        } /* end for */
	    } /* end if (catches) */

	    if (rs < 0) {
		for (i = (j-1) ; i >= 0 ; i -= 1) {
	  	    hsig = hp[i].sig ;
		    sap = &hp[i].action ;
		    u_sigaction(hsig,sap,nullptr) ;
		}
	        uc_free(iap->handles) ;
	        iap->handles = nullptr ;
	    } /* end if (error) */
	} /* end if (memory allocations) */

	if (rs >= 0) {
	    iap->magic = SIGMAN_MAGIC ;
	}

	return rs ;
}
/* end subroutine (sigman_start) */

int sigman_finish(sigman *iap) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (iap == nullptr) return SR_FAULT ;
	if (iap->magic != SIGMAN_MAGIC) return SR_NOTOPEN ;

	if (iap->handles != nullptr) {
	    SIGACTION	*sap ;
	    for (int i = (iap->nhandles-1)  ; i >= 0 ; i -= 1) {
	        cint	hsig = iap->handles[i].sig ;
	        sap = &iap->handles[i].action ;
	        rs1 = u_sigaction(hsig,sap,nullptr) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end for */
	    rs1 = uc_free(iap->handles) ;
	    if (rs >= 0) rs = rs1 ;
	    iap->handles = nullptr ;
	} /* end if */

	if (iap->nblocks > 0) {
	    pthread_sigmask(SIG_SETMASK,&iap->osm,nullptr) ;
	}

	iap->magic = 0 ;
	return rs ;
}
/* end subroutine (sigman_finish) */


