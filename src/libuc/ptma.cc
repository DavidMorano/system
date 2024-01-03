/* ptma HEADER */
/* lang=C++20 */

/* POSIX® Thread Mutex Attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a cleaned up version of the p-threads mutex-attribute set of
	subroutines (object).

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<pthread.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"ptma.h"


/* local defines */

#if	defined(SYSHAS_MUTEXROBUST) && (SYSHAS_MUTEXROBUST > 0)
#define	F_MUTEXROBUST	1
#else
#define	F_MUTEXROBUST	0
#endif


/* external subroutines */


/* forward references */

int		ptma_create(PTMA *) noex ;


/* local variables */

constexpr bool	f_mutexrobust = F_MUTEXROBUST ;


/* exported subroutines */

int ptma_create(PTMA *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    bool	f_exit = FALSE ;
	    repeat {
	        if ((rs = pthread_mutexattr_init(op)) > 0) rs = (- rs) ;
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
/* end subroutine (ptma_create) */

int ptma_destroy(PTMA *op) noex {
	int		rs ;
	rs = pthread_mutexattr_destroy(op) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_destroy) */

int ptma_getprioceiling(PTMA *op,int *oldp) noex {
	int		rs ;
	rs = pthread_mutexattr_getprioceiling(op,oldp) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_getprioceiling) */

int ptma_setprioceiling(PTMA *op,int fn) noex {
	int		rs ;
	rs = pthread_mutexattr_setprioceiling(op,fn) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_setprioceiling) */

int ptma_getprotocol(PTMA *op,int *oldp) noex {
	int		rs ;
	rs = pthread_mutexattr_getprotocol(op,oldp) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_getprotocol) */

int ptma_setprotocol(PTMA *op,int fn) noex {
	int		rs ;
	rs = pthread_mutexattr_setprotocol(op,fn) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_setprotocol) */

int ptma_getpshared(PTMA *op,int *oldp) noex {
	int		rs ;
	rs = pthread_mutexattr_getpshared(op,oldp) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_getpshared) */

int ptma_setpshared(PTMA *op,int fn) noex {
	int		rs ;
	rs = pthread_mutexattr_setpshared(op,fn) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_setpshared) */

int ptma_getrobustnp(PTMA *op,int *oldp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if constexpr (f_mutexrobust) {
	        rs = pthread_mutexattr_getrobust_np(op,oldp) ;
	        if (rs > 1) rs = (- rs) ;
	    } else {
	        (void) oldp ;
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_getrobustnp) */

int ptma_setrobustnp(PTMA *op,int fn) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if constexpr (f_mutexrobust) {
		rs = pthread_mutexattr_setrobust_np(op,fn) ;
		if (rs > 0) rs = (- rs) ;
	    } else {
	        (void) fn ;
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptma_setrobustnp) */

int ptma_gettype(PTMA *op,int *oldp) noex {
	int		rs ;
	rs = pthread_mutexattr_gettype(op,oldp) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_gettype) */

int ptma_settype(PTMA *op,int nf) noex {
	int		rs ;
	rs = pthread_mutexattr_settype(op,nf) ;
	if (rs > 0) rs = (- rs) ;
	return rs ;
}
/* end subroutine (ptma_settype) */


