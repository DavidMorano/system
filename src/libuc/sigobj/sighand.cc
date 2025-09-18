/* sighand SUPPORT */
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

  	Object:
	sighand

	Description:
	This small object provides a way to manage (block, ignore,
	and catch) process signals.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/param.h>
#include	<unistd.h>
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

#include	"sighand.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	SH		sighand
#define	SH_H		sighand_handler


/* imported namespaces */

using libuc::libmem ;


/* local typedefs */


/* external subroutines */


/* exported variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int sighand_magic(SH *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SIGHAND_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sighand_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sighand_start(SH *iap,cint *blocks,cint *igns,cint *cats,SH_H hf) noex {
	int		rs = SR_FAULT ;
	if (iap) {
            sigset_t        nsm ;
            int             nhandles = 0 ;
            int             sz ;
            void            *p ;
            rs = memclear(iap) ; /* dangerous */
            if (hf == nullptr) {
                hf = cast_reinterpret<SH_H>(voidp(SIG_IGN)) ;
            }
    /* blocks */
            if ((rs >= 0) && (blocks != nullptr)) {
                int         i{} ; /* used afterwards */
                uc_sigsetempty(&nsm) ;
                for (i = 0 ; blocks[i] != 0 ; i += 1) {
                    uc_sigsetadd(&nsm,blocks[i]) ;
                }
                iap->nblocks = i ;
                pthread_sigmask(SIG_BLOCK,&nsm,&iap->osm) ;
            } /* end if (blocks) */
    
    /* calculate the allocations size */
            if (igns != nullptr) {
                for (int i = 0 ; igns[i] != 0 ; i += 1) {
                    nhandles += 1 ;
                }
            }
            if (cats != nullptr) {
                for (int i = 0 ; cats[i] != 0 ; i += 1) {
                    nhandles += 1 ;
                }
            }
            sz = (nhandles * szof(sighand_ha)) ;
            if ((rs >= 0) && (sz > 0) && ((rs = libmem.mall(sz,&p)) >= 0)) {
                sighand_ha      *hp = (sighand_ha *) p ;
                SIGACTION	san{} ;
                SIGACTION	*sap ;
                int		hsig ;
                int		j = 0 ;
                iap->handles = (sighand_ha *) p ;
                iap->nhandles = nhandles ;
                uc_sigsetempty(&nsm) ;
                if (igns != nullptr) {
                    for (int i = 0 ; igns[i] != 0 ; i += 1) {
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
                    for (int i = 0 ; cats[i] != 0 ; i += 1) {
                        hsig = cats[i] ;
                        hp[j].sig = hsig ;
                        sap = &hp[j].action ;
                        san.sa_sigaction = (sighand_handler) hf ;
                        san.sa_mask = nsm ;
                        san.sa_flags = (SA_SIGINFO | SA_RESTART) ;
                        rs = u_sigaction(hsig,&san,sap) ;
                        if (rs < 0) break ;
                        j += 1 ;
                    } /* end for */
    
                } /* end if (cats) */
                if (rs < 0) { /* error */
                    for (int i = (j-1) ; i >= 0 ; i -= 1) {
                        hsig = hp[i].sig ;
                        sap = &hp[i].action ;
                        u_sigaction(hsig,sap,nullptr) ;
                    }
                    libmem.free(iap->handles) ;
                    iap->handles = nullptr ;
                }
            } /* end if (memory allocations) */
            if (rs >= 0) {
                iap->magic = SIGHAND_MAGIC ;
            }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sighand_start) */

int sighand_finish(SH *iap) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sighand_magic(iap)) >= 0) {
	    if (iap->handles != nullptr) {
	        SIGACTION	*sap ;
	        int		hsig ;
	        for (int i = (iap->nhandles-1)  ; i >= 0 ; i -= 1) {
	            hsig = iap->handles[i].sig ;
	            sap = &iap->handles[i].action ;
	            rs1 = u_sigaction(hsig,sap,nullptr) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end for */
	        rs1 = libmem.free(iap->handles) ;
	        if (rs >= 0) rs = rs1 ;
	        iap->handles = nullptr ;
	    } /* end if */
	    if (iap->nblocks > 0)  {
	        pthread_sigmask(SIG_SETMASK,&iap->osm,nullptr) ;
	    }
	    iap->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sighand_finish) */


