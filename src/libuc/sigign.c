/* sigign */
/* lang=C20 */

/* manage process signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A�D� Morano
	This was created along with the DATE object.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This small object provides a way to manage (block, ignore,
	and catch) process signals.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<signal.h>
#include	<unistd.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"sigign.h"


/* local defines */

#define	SIGIGN_HANDLE	struct sigign_handle


/* external subroutines */


/* exported variables */


/* local subroutines */


/* forward references */


/* local variables */

static cint	sigouts[] = {
	SIGTTOU,
	0
} ;


/* exported subroutines */

int sigign_start(SIGIGN *iap,const int *ignores) noex {
	int		rs = SR_FAULT ;
	int		nhandles = 0 ;
	if (iap) {
	    rs = SR_OK ;
	    if (ignores == NULL) ignores = sigouts ;
	    memclear(iap) ;
	    if (ignores != NULL) {
	        int		i = 0 ;
	        for (i = 0 ; ignores[i] != 0 ; i += 1) {
		    nhandles += 1 ;
	        }
	        nhandles = i ;
	        if (nhandles > 0) {
	            cint	size = (nhandles * sizeof(SIGIGN_HANDLE)) ;
	            void	*vp ;
	            iap->nhandles = nhandles ;
	            if ((rs = uc_malloc(size,&vp)) >= 0) {
	                SIGACTION	san, *sap ;
	                sigset_t	nsm ;
	                iap->handles = (SIGIGN_HANDLE *) vp ;
	                uc_sigsetempty(&nsm) ;
	                if (ignores != NULL) {
	                    SIGIGN_HANDLE	*hp = iap->handles ;
	                    int		hsig ;
	                    for (i = 0 ; ignores[i] != 0 ; i += 1) {
	                        hsig = ignores[i] ;
	                        hp[i].sig = hsig ;
	                        sap = &hp[i].action ;
	                        memset(&san,0,sizeof(SIGACTION)) ;
	                        san.sa_handler = SIG_IGN ;
	                        san.sa_mask = nsm ;
	                        san.sa_flags = 0 ;
	                        rs = u_sigaction(hsig,&san,sap) ;
			        if (rs < 0) break ;
	                    } /* end for */
	                    if (rs < 0) {
	                        for (int j = (i-1) ; j >= 0 ; j -= 1) {
	                            hsig = hp[j].sig ;
	                            sap = &hp[j].action ;
	                            u_sigaction(hsig,sap,NULL) ;
	                        }
			    }
	                } /* end if (ignores) */
		        if (rs < 0) {
	      		    uc_free(iap->handles) ;
			    iap->handles = NULL ;
	 	         }
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

int sigign_finish(SIGIGN *iap) noex {
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
	                rs1 = u_sigaction(hsig,sap,NULL) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end for */
	            rs1 = uc_free(iap->handles) ;
	            if (rs >= 0) rs = rs1 ;
	            iap->handles = NULL ;
	        } /* end if */
	        iap->magic = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sigign_finish) */


