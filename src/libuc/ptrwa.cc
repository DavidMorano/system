/* ptrwa SUPPORT */
/* lang=C++20 */

/* POSIX® Thread Read-Write lock attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object provides operational attributes for the
	POSIX read-write lock.

	Side-note: The POXIX® read-write lock is broken on the Solaris
	platform and is not used any long.  So this object here
	gets little use now-a-days.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ptrwa.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ptrwa_create(ptrwa *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    bool	f_exit = FALSE ;
	    repeat {
	        if ((rs = pthread_rwlockattr_init(op)) > 0) rs = (- rs) ;
	        if (rs < 0) {
	            switch (rs) {
	            case SR_NOMEM:
	                if (to_nomem-- > 0) {
		            msleep(1000) ;
		        } else {
	                    f_exit = TRUE ;
		        }
	                break ;
		    case SR_INTR:
		        break ;
		    default:
		        f_exit = TRUE ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwa_create) */

int ptrwa_destroy(ptrwa *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_rwlockattr_destroy(op) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ptrwa_destroy) */

int ptrwa_getpshared(ptrwa *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_rwlockattr_getpshared(op,oldp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ptrwa_getpshared) */

int ptrwa_setpshared(ptrwa *op,int fl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_rwlockattr_setpshared(op,fl) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (ptrwa_setpshared) */


