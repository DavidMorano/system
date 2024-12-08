/* ucsigset SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<csignal>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_sigsetempty(sigset_t *sp) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if ((rs = sigemptyset(sp)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigsetempty) */

int uc_sigsetfill(sigset_t *sp) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if ((rs = sigfillset(sp)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigsetfill) */

int uc_sigsetadd(sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if ((rs = sigaddset(sp,sn)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigsetadd) */

int uc_sigsetdel(sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if ((rs = sigdelset(sp,sn)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigsetdel) */

int uc_sigsetismem(sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sn >= 0) {
	        if ((rs = sigismember(sp,sn)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigsetismem) */


