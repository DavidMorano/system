/* umask SUPPORT */
/* lang=C++20 */

/* UNIX® UMASK (file-creation-mask) management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David AÂ­DÂ­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module serves to provide two functions that manipulate
	the UNIX® UMASK.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<csignal>		/* |sig_atomic_t| */
#include	<climits>
#include	<cstring>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<localmisc.h>

#include	"umask.h"


/* local defines */


/* local namespaces */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external subroutines */


/* local structures */

struct umask {
	ptm		m ;		/* data mutex */
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
} ;


/* forward references */

extern "C" {
    static void	umask_atforkbefore() noex ;
    static void	umask_atforkafter() noex ;
    static void	umask_exit() noex ;
}


/* local variables */

static struct umask	umask_data ;


/* exported variables */


/* exported subroutines */

int umask_init() noex {
	struct umask	*uip = &umask_data ;
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! uip->f_void) {
	    rs = SR_OK ;
	    if (! uip->f_init) {
	        uip->f_init = true ;
	        if ((rs = ptm_create(&uip->m,nullptr)) >= 0) {
	            void_f	b = umask_atforkbefore ;
	            void_f	a = umask_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(umask_exit)) >= 0) {
	    	            uip->f_initdone = true ;
		            f = true ;
		        }
		        if (rs < 0) {
		            uc_atforkexpunge(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
	 	    if (rs < 0) {
		        ptm_destroy(&uip->m) ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            uip->f_init = false ;
		}
	    } else {
	        while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	            rs = msleep(1) ;
	            if (rs == SR_INTR) rs = SR_OK ;
	        }
	        if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	    }
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (umask_init) */

int umask_fini() noex {
	struct umask	*uip = &umask_data ;
	int		rs = SR_NXIO ;
	int		rs1 ;
	if (uip->f_initdone && (! uip->f_void)) {
	    uip->f_void = false ;
	    rs = SR_OK ;
	    {
	        void_f	b = umask_atforkbefore ;
	        void_f	a = umask_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&uip->m) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	    uip->f_initdone = false ;
	} /* end if (atexit registered) */
	return rs ;
}
/* end subroutine (umask_fini) */

int umaskget() noex {
	struct umask	*uip = &umask_data ;
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		cmask = 0 ;
	if ((rs = sigblocker_start(&b,nullptr)) >= 0) {
	    if ((rs = umask_init()) >= 0) {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = ptm_lock(&uip->m)) >= 0) { /* single */
			{
			    cmask = umask(0) ; /* in case of race! */
			    umask(cmask) ;
			}
	                rs1 = ptm_unlock(&uip->m) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	            rs1 = uc_forklockend() ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (forklock) */
	    } /* end if (init) */
	    rs1 = sigblocker_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */
	cmask &= INT_MAX ;
	return (rs >= 0) ? cmask : rs ;
}
/* end subroutine (umaskget) */

int umaskset(mode_t cmask) noex {
	struct umask	*uip = &umask_data ;
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		omask = 0 ;
	if ((rs = sigblocker_start(&b,nullptr)) >= 0) {
	    if ((rs = umask_init()) >= 0) {
	        if ((rs = uc_forklockbegin(-1)) >= 0) { /* multi */
	            if ((rs = ptm_lock(&uip->m)) >= 0) { /* single */
			{
			    omask = umask(cmask) ;
			}
	                rs1 = ptm_unlock(&uip->m) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex) */
	            rs1 = uc_forklockend() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (forklock) */
	    } /* end if (init) */
	    rs1 = sigblocker_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblock) */
	omask &= INT_MAX ;
	return (rs >= 0) ? omask : rs ;
}
/* end subroutine (umaskset) */


/* local subroutines */

static void umask_atforkbefore() noex {
	struct umask	*uip = &umask_data ;
	ptm_lock(&uip->m) ;
}
/* end subroutine (umask_atforkbefore) */

static void umask_atforkafter() noex {
	struct umask	*uip = &umask_data ;
	ptm_unlock(&uip->m) ;
}
/* end subroutine (umask_atforkafter) */

static void umask_exit() noex {
}
/* end subroutine (umask_exit) */


