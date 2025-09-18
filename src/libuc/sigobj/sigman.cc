/* sigman SUPPORT */
/* charset=ISO8859-1 */
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
#include	<sys/types.h>		/* system types */
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucsig.h>
#include	<localmisc.h>

#include	"sigman.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* memclear(2u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;


/* local typedefs */

typedef	sigman_ha *	handp ;


/* external subroutines */


/* exported variables */


/* local subroutines */


/* forward references */

template<typename ... Args>
static int sigman_ctor(sigman *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    sigman_head	 *hp = cast_static<sigman_head *>(op) ;
	    rs = memclear(hp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigman_ctor) */

static int sigman_dtor(sigman *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigman_dtor) */

template<typename ... Args>
static inline int sigman_magic(sigman *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SIGMAN_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sigman_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sigman_start(sigman *op,cint *blks,cint *igns,cint *cats,
		sigmanhand_f handle) noex {
	int		rs ;
	if ((rs = sigman_ctor(op)) >= 0) {
	    sigset_t	nsm ;
	    int		i ;
	    int		nhs = 0 ;
	    int		sz ;
	    void	*p ;
	    if (handle == nullptr) {
	        handle = cast_reinterpret<sigmanhand_f>(SIG_IGN) ;
	    }
	    if ((rs >= 0) && blks) {
	        uc_sigsetempty(&nsm) ;
	        for (i = 0 ; blks[i] != 0 ; i += 1) {
	            uc_sigsetadd(&nsm,blks[i]) ;
	        }
	        op->nblks = i ;
	        pthread_sigmask(SIG_BLOCK,&nsm,&op->osm) ;
	    } /* end if (blks) */
	    /* calculate the allocations size */
	    if (igns) {
	        for (i = 0 ; igns[i] != 0 ; i += 1) {
	            nhs += 1 ;
	        }
	    }
	    if (cats) {
	        for (i = 0 ; cats[i] != 0 ; i += 1) {
	            nhs += 1 ;
	        }
	    }
	    sz = (nhs * szof(sigman_ha)) ;
	    if ((rs >= 0) && (nhs > 0) && ((rs = libmem.mall(sz,&p)) >= 0)) {
	        sigman_ha	*hp = handp(p) ;
	        SIGACTION	san{} ;
	        SIGACTION	*sap ;
	        int		hsig ;
	        int		j = 0 ;
	        op->handles = handp(p) ;
	        op->nhs = nhs ;
	        /* ignore these signals */
	        uc_sigsetempty(&nsm) ;
	        if (igns) {
	            for (i = 0 ; igns[i] != 0 ; i += 1) {
	                hsig = igns[i] ;
	                hp[j].sig = hsig ;
	                sap = &hp[j].action ;
	                san.sa_handler = SIG_IGN ;
	                san.sa_mask = nsm ;
	                san.sa_flags = 0 ;
	                rs = u_sigaction(hsig,&san,sap) ;
	                if (rs < 0) break ;
	                j += 1 ;
	            } /* end for */
	        } /* end if (igns) */
	        /* catch (interrupt on) these signals */
	        if ((rs >= 0) && (cats != nullptr)) {
	            for (i = 0 ; cats[i] != 0 ; i += 1) {
	                hsig = cats[i] ;
	                hp[j].sig = hsig ;
	                sap = &hp[j].action ;
	                memclear(&san) ;
	                san.sa_handler = handle ;
	                san.sa_mask = nsm ;
	                san.sa_flags = 0 ;
	                rs = u_sigaction(hsig,&san,sap) ;
	                if (rs < 0) break ;
	                j += 1 ;
	            } /* end for */
	        } /* end if (cats) */
	        if (rs < 0) {
		    for (i = (j-1) ; i >= 0 ; i -= 1) {
	  	        hsig = hp[i].sig ;
		        sap = &hp[i].action ;
		        u_sigaction(hsig,sap,nullptr) ;
		    } /* end if */
	            libmem.free(op->handles) ;
	            op->handles = nullptr ;
	        } /* end if (error) */
	    } /* end if (memory allocations) */
	    if (rs >= 0) {
	        op->magic = SIGMAN_MAGIC ;
	    }
	    if (rs < 0) {
		sigman_dtor(op) ;
	    }
	} /* end if (sigman_ctor) */
	return rs ;
}
/* end subroutine (sigman_start) */

int sigman_finish(sigman *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sigman_magic(op)) >= 0) {
	    if (op->handles) {
	        SIGACTION	*sap ;
	        for (int i = (op->nhs-1)  ; i >= 0 ; i -= 1) {
	            cint	hsig = op->handles[i].sig ;
	            sap = &op->handles[i].action ;
	            rs1 = u_sigaction(hsig,sap,nullptr) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end for */
	        rs1 = libmem.free(op->handles) ;
	        if (rs >= 0) rs = rs1 ;
	        op->handles = nullptr ;
	    } /* end if */
	    if (op->nblks > 0) {
	        rs1 = pthread_sigmask(SIG_SETMASK,&op->osm,nullptr) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = sigman_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sigman_finish) */


