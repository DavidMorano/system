/* ptrwlock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* POSIX® Thread Read-Write Lock (PTRWLOCK) */
/* version %I% last-modified %G% */
/* ** broken on SOLARIS®! ** */


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
	is broken.  See alternative implementations.  This module
	provides a sanitized version of the standard POSIX®
	reader-writer lock facility provided with some new UNIX®i.
	Some operating system problems are managed within these
	routines for the common stuff that happens when a poorly
	configured OS gets overloaded!  Enjoy!

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
#include	<pthread.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>
#include        <errtimer.hh>
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

constexpr int		mint = (1000 / NLPS) ;


/* exported variables */


/* exported subroutines */

int ptrwlock_create(ptrwlock *psp,ptrwa *atp) noex {
	int		rs = SR_FAULT ;
	if (psp) ylikely {
	    repeat {
	        if ((rs = pthread_rwlock_init(psp,atp)) > 0) {
		    rs = (- rs) ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_create) */

int ptrwlock_destroy(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) ylikely {
	    repeat {
	        if ((rs = pthread_rwlock_destroy(psp)) > 0) {
		    rs = (- rs) ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_destroy) */

int ptrwlock_lockrd(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) ylikely {
	    repeat {
	        if ((rs = pthread_rwlock_rdlock(psp)) > 0) {
		    rs = (- rs) ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_lockrd) */

int ptrwlock_lockrdtry(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) ylikely {
	    repeat {
	        if ((rs = pthread_rwlock_tryrdlock(psp)) > 0) {
		    rs = (- rs) ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_lockrdtry) */

int ptrwlock_lockrdto(ptrwlock *psp,int to) noex {
	int		rs = SR_FAULT ;
	if (to < 0) to = (INT_MAX / (2 * NLPS)) ;
	if (psp) ylikely {
	    int		cto = (to*NLPS) ;
	    int		c = 0 ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = pthread_rwlock_tryrdlock(psp)) > 0) {
		    rs = (- rs) ;
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
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		    f_exit = true ;
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_lockrdto) */

int ptrwlock_lockwr(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) ylikely {
	    repeat {
	        if ((rs = pthread_rwlock_wrlock(psp)) > 0) {
		    rs = (- rs) ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_lockwr) */

int ptrwlock_lockwrtry(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) ylikely{
	    repeat {
	        if ((rs = pthread_rwlock_trywrlock(psp)) > 0) {
		    rs = (- rs) ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_lockwrtry) */

int ptrwlock_lockwrto(ptrwlock *psp,int to) noex {
	int		rs = SR_FAULT ;
	if (to < 0) to = (INT_MAX / (2 * NLPS)) ;
	if (psp) ylikely{
	    int		cto = (to*NLPS) ;
	    int		c = 0 ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = pthread_rwlock_trywrlock(psp)) > 0) {
		    rs = (- rs) ;
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
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		    f_exit = true ;
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_lockwrto) */

int ptrwlock_lockend(ptrwlock *psp) noex {
	int		rs = SR_FAULT ;
	if (psp) ylikely{
	    repeat {
	        if ((rs = pthread_rwlock_unlock(psp)) > 0) {
		    rs = (- rs) ;
		} else if (rs < 0) {
		    rs = SR_NOANODE ;
		}
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ptrwlock_lockend) */

int ptrwlock::lockrdto(int a) noex {
    	return ptrwlock_lockrdto(this,a) ;
}

int ptrwlock::lockwrto(int a) noex {
    	return ptrwlock_lockwrto(this,a) ;
}

void ptrwlock::dtor() noex {
	if (cint rs = ptrwlock_destroy(this) ; rs < 0) {
	    ulogerror("ptrwlock",rs,"dtor-destroy") ;
	}
} /* end method (ptrwlock::dtor) */

int ptrwlock_cr::operator () (ptrwa *ap) noex {
    	int		rs ;
	if ((rs = ptrwlock_create(op,ap)) >= 0) {
	    op->magic = PTRWLOCK_MAGIC ;
	}
	return rs ;
} /* end method (ptrwlock_cr::operator) */

ptrwlock_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case ptrwlockmem_destroy:
	        rs = ptrwlock_destroy(op) ;
	        op->magic = PTRWLOCK_MAGIC ;
	        break ;
	    case ptrwlockmem_lockrd:
	        rs = ptrwlock_lockrd(op) ;
	        break ;
	    case ptrwlockmem_lockrdtry:
	        rs = ptrwlock_lockrdtry(op) ;
	        break ;
	    case ptrwlockmem_lockwr:
	        rs = ptrwlock_lockwr(op) ;
	        break ;
	    case ptrwlockmem_lockwrtry:
	        rs = ptrwlock_lockwrtry(op) ;
	        break ;
	    case ptrwlockmem_lockend:
	        rs = ptrwlock_lockend(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (ptrwlock_co::operator) */


