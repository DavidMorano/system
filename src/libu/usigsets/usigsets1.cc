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
#include	<pthread.h>		/* POSIX |pthread_setmask(3c)| */
#include	<cerrno>		/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

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

usigsets	usigseter ;


/* exported subroutines */

extern "C" {
    int u_sigsetempty	(sigset_t *sp) noex {
	return usigseter.empty(sp) ;
    }
    int u_sigsetfill	(sigset_t *sp) noex {
	return usigseter.fill(sp) ;
    }
    int u_sigsetadd	(sigset_t *sp,int si) noex {
	return usigseter.add(sp,si) ;
    }
    int u_sigsetdel	(sigset_t *sp,int si) noex {
	return usigseter.del(sp,si) ;
    }
    int u_sigsetismem	(const sigset_t *sp,int si) noex {
	return usigseter.ismem(sp,si) ;
    }
} /* end extern (C) */

int u_sigmask(int w,usigset *nsp,usigset *osp) noex {
    int		rs ;
    if (nsp && osp) {
        rs = u_sigmask(w,&nsp->ss,&osp->ss) ;
    } else if (nsp) {
	sigset_t *tosp = nullptr ;
        rs = u_sigmask(w,&nsp->ss,tosp) ;
    } else {
	sigset_t *tnsp = nullptr ;
	sigset_t *tosp = nullptr ;
        rs = u_sigmask(w,tnsp,tosp) ;
    }
    return rs ;
} /* end subroutine */

/* local subroutines */
int u_sigmask(int w,usigset *nsp,sigset_t *osp) noex {
    int		rs ;
    if (nsp) {
        rs = u_sigmask(w,&nsp->ss,osp) ;
    } else {
	sigset_t *tnsp = nullptr ;
        rs = u_sigmask(w,tnsp,osp) ;
    }
    return rs ;
} /* end subroutine */

int u_sigmask(int w,sigset_t *nsp,usigset *osp) noex {
    int		rs ;
    if (osp) {
        rs = u_sigmask(w,nsp,&osp->ss) ;
    } else {
	sigset_t *tosp = nullptr ;
        rs = u_sigmask(w,nsp,tosp) ;
    }
    return rs ;
} /* end subroutine */

int u_sigwait(usigset *sp,int *rp) noex {
    return u_sigwait(&sp->ss,rp) ;
} /* end subroutine */


/* local subroutines */

int usigsets::empty(sigset_t *sp) noex {
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    if ((rs = sigemptyset(sp)) < 0) {
	        rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::empty) */

int usigsets::fill(sigset_t *sp) noex {
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    if ((rs = sigfillset(sp)) < 0) {
	        rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::fill) */

int usigsets::add(sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sn >= 0) ylikely {
	        if ((rs = sigaddset(sp,sn)) < 0) {
	            rs = (neg errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::add) */

int usigsets::del(sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sn >= 0) ylikely {
	        if ((rs = sigdelset(sp,sn)) < 0) {
	            rs = (neg errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::del) */

int usigsets::ismem(const sigset_t *sp,int sn) noex {
	int		rs = SR_FAULT ;
	if (sp) ylikely {
	    rs = SR_INVALID ;
	    if (sn >= 0) ylikely {
	        if ((rs = sigismember(sp,sn)) < 0) {
		    rs = (neg errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (usigsets::ismem) */


