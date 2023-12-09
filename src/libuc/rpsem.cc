/* psem */
/* lang=C++20 */

/* Posix Semaphore (PSEM) */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This module provides a sanitized version of the standard POSIX®
        semaphore facility provided with some new UNIX®i. Some operating system
        problems are managed within these routines for the common stuff that
        happens when a poorly configured OS gets overloaded!

	Enjoy!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<semaphore.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>
#include	<usupport.h>

#include	"psem.h"


/* local defines */

#undef	NLPS
#define	NLPS		5	/* number of polls per second */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int psem_create(PSEM *psp,int pshared,int count) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    cint	ic = int(count & INT_MAX) ;
	    repeat {
	        if ((rs = sem_init(psp,pshared,ic)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_create) */

int psem_destroy(PSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = sem_destroy(psp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_destroy) */

#ifdef	COMMENT
int psem_getvalue(PSEM *psp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (psp) {
	    repeat {
	        if ((rs = sem_getvalue(psp,&c)) < 0) {
		    rs = (- errno) ;
	        } else {
		    c &= INT_MAX ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return (rs >= 0) c : rs ;
}
/* end subroutine (psem_getvalue) */
#endif /* COMMENT */

int psem_waiti(PSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	        repeat {
	        if ((rs = sem_wait(psp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_waiti) */

int psem_wait(PSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = sem_wait(psp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_wait) */

int psem_trywait(PSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = sem_trywait(psp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_trywait) */

int psem_waiter(PSEM *psp,int to) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (to < 0) to = (INT_MAX/(2*NLPS)) ;
	if (psp) {
	    cint	mint = (1000/NLPS) ;
	    int		cto = (to*NLPS) ;
	    bool		f_exit = FALSE ;
	    repeat {
	        if ((rs = sem_trywait(psp)) < 0) rs = (- errno) ;
	        if (rs < 0) {
		    switch (rs) {
	    	    case SR_AGAIN:
		        if (c++ < cto) {
			    msleep(mint) ;
		        } else {
			    rs = SR_TIMEDOUT ;
			    f_exit = true ;
		        }
		        break ;
		    case SR_INTR:
		        break ;
		    default:
		        f_exit = true ;
		        break ;
		    } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_waiter) */

int psem_post(PSEM *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = sem_post(psp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (psem_post) */


