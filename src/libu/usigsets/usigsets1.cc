/* ucsigsets1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 2000,2020 David A­D­ Morano.  All rights reserved. */

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<pthread.h>		/* |pthread_setmask(3c)| */
#include	<cerrno>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>

module usigsets ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

usigsets	usigset ;


/* exported subroutines */

extern "C" {
    int u_sigsetempty	(sigset_t *sp) noex {
	return usigset.empty(sp) ;
    }
    int u_sigsetfill	(sigset_t *sp) noex {
	return usigset.fill(sp) ;
    }
    int u_sigsetadd	(sigset_t *sp,int si) noex {
	return usigset.add(sp,si) ;
    }
    int u_sigsetdel	(sigset_t *sp,int si) noex {
	return usigset.del(sp,si) ;
    }
    int u_sigsetismem	(sigset_t *sp,int si) noex {
	return usigset.ismem(sp,si) ;
    }
} /* end extern (C) */


/* local subroutines */

int usigsets::empty(sigset_t *sp) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if ((rs = sigemptyset(sp)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::empty) */

int usigsets::fill(sigset_t *sp) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    if ((rs = sigfillset(sp)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::fill) */

int usigsets::add(sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sn >= 0) {
	        if ((rs = sigaddset(sp,sn)) < 0) {
	            rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::add) */

int usigsets::del(sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) {
	    rs = SR_INVALID ;
	    if (sn >= 0) {
	        if ((rs = sigdelset(sp,sn)) < 0) {
	            rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::del) */

int usigsets::ismem(const sigset_t *sp,int sn) noex {
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
} /* end method (usigsets::ismem) */


