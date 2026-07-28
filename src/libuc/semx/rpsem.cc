/* rpsem SUPPORT ("real" POSIX® Semaphore) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX© unnamed Semaphore (PSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	rpsem

	Description:
	This module provides a sanitized version of the standard
	POSIX® semaphore facility provided with some new UNIX®i.
	Some operating system problems are managed within these
	routines for the common stuff that happens when a poorly
	configured OS gets overloaded!

	Enjoy!

	Important-note:
	The Apple-Darwin operating system has the unnamed POSIX®
	semaphores depracated. This produces bad and very annoying
	error messages to appear from the compiler when attempting
	to use the |init| and |destroy| subroutines (methods) of
	the POSIX® semaphore object.  There is no easy fix for this
	that I know of.  So to get around this,  I redefine (using
	the C/C++ language preprocessor (CPP) to redefine the
	names for those subroutines to something else (which are
	defined elsewhere) so that at least we get a clear ocmpile
	for code test purposes.  Not that for real run-time, the
	native Apple-Darwin implementation of POSIX® is completely
	bypassed with my own implementation.  See the pre-processor
	definitions under the section below named "local defines"
	to see how I deal with this (crap)!  

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<semaphore.h>		/* POSIX® semaphores */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cassert>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"rpsem.h"


/* local defines */

/****
This "define" crap below is needed in order to avoid a compiler
error (on Apple-Darwin).  Apple-Darwin has marked the two subroutine
callas below as "depracated."  So in order to avoid that crap, I need to
do the following.
****/

#if	(!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0)
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#define	sem_init	darwinsem_init
#define	sem_destroy	darwinsem_destroy
#endif
#endif /* (!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0) */

#undef	NLPS
#define	NLPS		5	/* number of polls per second */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int rpsem_magic(rpsem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == RPSEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (rpsem_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int rpsem_create(rpsem *op,int pshared,int acnt) noex {
	int		rs = SR_FAULT ;
	assert(op) ;
	if (op) ylikely {
	    rs = SR_INVALID ;
	    if (acnt > 0) {
	        repeat {
	            if ((rs = sem_init(&op->ps,pshared,acnt)) < 0) {
		        rs = (neg errno) ;
	            }
	        } until (rs != SR_INTR) ;
		if (rs >= 0) {
		    op->magval = RPSEM_MAGIC ;
		} /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (rpsem_create) */

int rpsem_destroy(rpsem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = rpsem_magic(op)) >= 0) ylikely {
	    repeat {
	        if ((rs = sem_destroy(&op->ps)) < 0) {
		    rs = (neg errno) ;
	        }
	    } until (rs != SR_INTR) ;
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (rpsem_destroy) */

int rpsem_wait(rpsem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = rpsem_magic(op)) >= 0) ylikely {
	    repeat {
	        if ((rs = sem_wait(&op->ps)) < 0) {
		    rs = (neg errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (rpsem_wait) */

int rpsem_trywait(rpsem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = rpsem_magic(op)) >= 0) ylikely {
	    repeat {
	        if ((rs = sem_trywait(&op->ps)) < 0) {
		    rs = (neg errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (rpsem_trywait) */

int rpsem_waiter(rpsem *op,int to) noex {
	int		rs ;
	int		c = 0 ;
	assert(op) ;
	if (to < 0) {
	    to = (INT_MAX / (2 * NLPS)) ;
	}
	if ((rs = rpsem_magic(op)) >= 0) ylikely {
	    cint	mint = (1000 / NLPS) ;
	    cint	cto = (to * NLPS) ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = sem_trywait(&op->ps)) < 0) {
		    rs = (neg errno) ;
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
	} /* end if (magic) */
	return rs ;
} /* end subroutine (rpsem_waiter) */

int rpsem_post(rpsem *op) noex {
	int		rs ;
	assert(op) ;
	if ((rs = rpsem_magic(op)) >= 0) ylikely {
	    repeat {
	        if ((rs = sem_post(&op->ps)) < 0) {
		    rs = (neg errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (rpsem_post) */

int rpsem_count(rpsem *op) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	assert(op) ;
	if ((rs = rpsem_magic(op)) >= 0) ylikely {
	    while ((rs = rpsem_trywait(op)) >= 0) {
		c += 1 ;
	    } /* end while */
	    if (rs == SR_AGAIN) {
		rs = SR_OK ;
		for (int i = 0 ; (rs >= 0) && (i < c) ; i += 1) {
		    rs = rpsem_post(op) ;
		} /* end for */
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (rpsem_count) */


/* local subroutines */

int rpsem::create(int pshared,int acnt) noex {
	return rpsem_create(this,pshared,acnt) ;
}

rpsem::operator int () noex {
	return rpsem_count(this) ;
}

void rpsem::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("rpsem",rs,"fini-destroy") ;
	}
} /* end method (rpsem::dtor) */

int rpsem_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case rpsemmem_wait:
	        rs = rpsem_wait(op) ;
	        break ;
	    case rpsemmem_waiter:
	        rs = rpsem_waiter(op,a) ;
	        break ;
	    case rpsemmem_trywait:
	        rs = rpsem_trywait(op) ;
	        break ;
	    case rpsemmem_post:
	        rs = rpsem_post(op) ;
	        break ;
	    case rpsemmem_count:
	        rs = rpsem_count(op) ;
	        break ;
	    case rpsemmem_destroy:
	        rs = rpsem_destroy(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (rpsem_co::operator) */


