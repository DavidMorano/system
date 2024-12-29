/* ptrwlock SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */
/* ** broken on SOLARIS®! ** */

/* POSIX® Thread Read-Write Lock (PTRWLOCK) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ptrwlock
	
	** broken on SOLARIS®! **
	Description:
	The supplied reader-writer lock implemetation (used within
	this source file) on Solaris® (used within this source file)
	is broken.  See alternative implementations.

	This module provides a sanitized version of the standard
	POSIX® reader-writer lock facility provided with some new
	UNIX®i.  Some operating system problems are managed within
	these routines for the common stuff that happens when a
	poorly configured OS gets overloaded!

	Enjoy!

	Important note:

	** broken on SOLARIS®! **

	Use a working read-write lock, like for example LOCKRW by
	David A­D­ Morano.

	Updated note:
	This object (or rather the underlying RW-LOCK facility
	itself) is **broken** on Solaris®.  It is rather complicated
	but when this object is used within a shared object that
	was loaded by a program manually (through |dlopen(3dl)|)
	it fails by messing up its lock state for some reason.  This
	failure, although quite obscure for most, makes this object
	unusable for any of our purposes.  Use my own read-write
	lock (which I developed myself) instead.  I do not make
	stupid-ass mistakes like the Solaris® developers do.  Maybe
	I make my own types of mistakes, but not the stupid-ass
	mistakes that the Solaris® developers have done.
		-- David A­D­ Morano

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<pthread.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"ptrwlock.h"
#include	"ptrwa.h"


/* local defines */

#undef	NLPS
#define	NLPS		2


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	mint = (1000/NLPS) ;


/* exported variables */


/* exported subroutines */

int ptrwlock_create(ptrwlock *psp,ptrwa *atp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_init(psp,atp)) > 0) {
		    rs = (- rs) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_create) */

int ptrwlock_destroy(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_destroy(psp)) > 0) {
		    rs = (- rs) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_destroy) */

int ptrwlock_rdlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_rdlock(psp)) > 0) {
		    rs = (- rs) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_rdlock) */

int ptrwlock_tryrdlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_tryrdlock(psp)) > 0) {
		    rs = (- rs) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_tryrdlock) */

int ptrwlock_rdlockto(ptrwlock *psp,int to) noex {
	int		rs = SR_FAULT ;
	if (to < 0) to = (INT_MAX/(2*NLPS)) ;
	if (psp) {
	    int		cto = (to*NLPS) ;
	    int		c = 0 ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = pthread_rwlock_tryrdlock(psp)) > 0) {
		    rs = (- rs) ;
		}
	        if (rs < 0) {
		    switch (rs) {
		    case SR_BUSY:
		        if (++c < cto) {
	    		    msleep(mint) ;
		        } else {
			    f_exit = true ;
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
	        if ((rs = pthread_rwlock_wrlock(psp)) > 0) {
		   rs = (- rs) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_wrlock) */

int ptrwlock_trywrlock(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    repeat {
	        if ((rs = pthread_rwlock_trywrlock(psp)) > 0) {
		    rs = (- rs) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_trywrlock) */

int ptrwlock_wrlockto(ptrwlock *psp,int to) noex {
	int		rs = SR_FAULT ;
	if (to < 0) to = (INT_MAX/(2*NLPS)) ;
	if (psp) {
	    int		cto = (to*NLPS) ;
	    int		c = 0 ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = pthread_rwlock_trywrlock(psp)) > 0) {
		    rs = (- rs) ;
		}
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
	        if ((rs = pthread_rwlock_unlock(psp)) > 0) {
		    rs = (- rs) ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_unlock) */


