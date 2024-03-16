/* ptrwlock SUPPORT */
/* lang=C++20 */
/* ** broken on SOLARIS�! ** */

/* POSIX� Thread Read-Weite Lock (PRWLOCK) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	** broken on SOLARIS�! **
	The supplied reader-writer lock implemetation (used within
	this source file) on Solaris (used within this source file)
	is broken.  See alternative implementionals.

	This module provides a sanitized version of the standard
	POSIX� semaphore facility provided with some new UNIX�i.
	Some operating system problems are managed within these
	routines for the common stuff that happens when a poorly
	configured OS gets overloaded!

	Enjoy!

	Important note:

	** broken on SOLARIS�! **

	Use a working read-write lock, like for example LOCKRW by
	David A�D� Morano.

	Updated note:

	This object (or rather the underlying RW-LOCK facility
	itself) is **broken** on Solaris�.  It is rather complicated
	but when this object is used within a shared object that
	was loaded by a program manually (through |dlopen(3dl)|)
	it fails by messing up its lock state for some reason.  This
	failure, although quite obscure for most, makes this object
	unusable for any of our purposes.  Use our own read-write
	lock (which we developed ourselves) instead.  We do not
	make stupid-ass mistakes like the Solaris� developers do.
	Maybe we make our own types of mistakes, but not the
	stupid-ass mistakes that the Solaris� developers have done.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<pthread.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ptrwlock.h"
#include	"ptrwa.h"


/* local defines */

#undef	NLPS
#define	NLPS		2


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ptrwlock_create(ptrwlock *psp,ptrwa *atp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_init(psp,atp)) > 0) rs = (- rs) ;
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_create) */

int ptrwlock_destroy(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_destroy(psp)) > 0) rs = (- rs) ;
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_destroy) */

int ptrwlock_rdlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_rdlock(psp)) > 0) rs = (- rs) ;
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_rdlock) */

int ptrwlock_tryrdlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_tryrdlock(psp)) > 0) rs = (- rs) ;
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_tryrdlock) */

int ptrwlock_rdlockto(ptrwlock *psp,int to) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    cint	mint = (1000/NLPS) ;
	    int		cto ;
	    int		c = 0 ;
	    bool	f_exit = false ;
	    if (to < 0) to = (INT_MAX/(2*NLPS)) ;
	    cto = (to*NLPS) ;
	    repeat {
	        if ((rs = pthread_rwlock_tryrdlock(psp)) > 0) rs = (- rs) ;
	        if (rs < 0) {
		    switch (rs) {
		    case SR_BUSY:
		        if (++c < cto) {
	    		    msleep(mint) ;
		        } else {
			    f_exit = false ;
		        }
		        break ;
		    case SR_INTR:
		        break ;
		    default:
		        f_exit = true ;
		        break ;
	            } /* end siwtch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_rdlockto) */

int ptrwlock_wrlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_wrlock(psp)) > 0) rs = (- rs) ;
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_wrlock) */

int ptrwlock_trywrlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_trywrlock(psp)) > 0) rs = (- rs) ;
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_trywrlock) */

int ptrwlock_wrlockto(ptrwlock *psp,int to) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    cint	mint = (1000/NLPS) ;
	    int		cto ;
	    int		c = 0 ;
	    bool	f_exit = false ;
	    if (to < 0) to = (INT_MAX/(2*NLPS)) ;
	    cto = (to*NLPS) ;
	    repeat {
	        if ((rs = pthread_rwlock_trywrlock(psp)) > 0) rs = (- rs) ;
	        if (rs < 0) {
		    switch (rs) {
		    case SR_BUSY:
		        if (++c < cto) {
	    	            msleep(mint) ;
		        } else {
	    	            f_exit = true ;
		        }
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
/* end subroutine (ptrwlock_wrlockto) */

int ptrwlock_unlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_unlock(psp)) > 0) rs = (- rs) ;
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_unlock) */


