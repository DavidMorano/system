/* psem SUPPORT */
/* lang=C++20 */

/* POSIX© unnamed Semaphore (PSEM) */
/* version %I% last-modified %G% */

#define	CF_TESTNATIVE	0		/* test-only using native facility */

/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
	that I know of.  So to get around this, I redefine (using
	the C/C++ language preprocessor (CPP) to redefine the
	names for those subroutines to something else (which are
	defined) elsewhere ) so that at least we get a clear ocmpile
	for code test purposes.  Not that for real run-time, the
	native Apple-Darwin implementation of POSIX® is completely
	bypassed with my own implementation. See the pre-processor
	definitions under the section below named "local defines"
	to see how I deal with this (crap)!  

*******************************************************************************/

#ifndef	CF_TESTNATIVE
#define	CF_TESTNATIVE	0
#endif

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<semaphore.h>
#include	<cerrno>
#include	<climits>
#include	<usystem.h>
#include	<localmisc.h>
#include	<usupport.h>

#if	CF_TESTNATIVE
#include	"rpsem.h"
#else /* CF_TESTNATIVE */
#include	"psem.h"
#endif /* CF_TESTNATIVE */

#if	(!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0)
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#define	sem_init	darwinsem_init
#define	sem_destroy	darwinsem_destroy
#endif
#endif /* (!defined(SYSHAS_PSEM)) || (SYSHAS_PSEM == 0) */


/* local defines */

#undef	NLPS
#define	NLPS		5	/* number of polls per second */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int psem_create(psem *psp,int pshared,int count) noex {
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

int psem_destroy(psem *psp) noex {
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
int psem_getvalue(psem *psp) noex {
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

int psem_waiti(psem *psp) noex {
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

int psem_wait(psem *psp) noex {
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

int psem_trywait(psem *psp) noex {
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

int psem_waiter(psem *psp,int to) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (to < 0) to = (INT_MAX/(2*NLPS)) ;
	if (psp) {
	    cint	mint = (1000/NLPS) ;
	    int		cto = (to*NLPS) ;
	    bool		f_exit = false ;
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

int psem_post(psem *psp) noex {
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


