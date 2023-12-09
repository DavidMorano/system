/* lockrw */
/* lang=C++20 */

/* Reader-Writer Lock (LOCKRW) */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-08-19, David A­D­ Morano
	This module was originally written because the read-write
	lock facility on Solaris® was broken when inside of a
	dynamically loaded module.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	The Solaris® implmentation of the POSIX® read-write lock
	is BROKEN!  We can all thank the "smart" Solaris® developers
	for that.  How did Solaris® manage to create a borken
	implementation?  Firstly, they thought that they were really
	smart people.  Secondly, from their own literature, they
	created a read-write lock that was especially "optimized,"
	whatever that means. They forgot that it was more important
	to create a *working* version of a piece of code, than a
	fast *broken* version of that code.

	This module provides a working (and minimally smart)
	read-write lock.

	Notes:

	+ For those interested, the whole logic of this lock facility
	is pretty much embodied in the subroutine (below)
	|lockrw_notready()|. One sort of wonders why the "smart"
	Solaris® developers could not have just made a straight-forward
	implementation along these same lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<time.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"lockrw.h"


/* local defines */


/* external subroutines */


/* forward references */

static int	lockrw_ptminit(lockrw *,int) noex ;
static int	lockrw_ptcinit(lockrw *,int) noex ;
static int	lockrw_notready(lockrw *,int) noex ;


/* local variables */


/* exported subroutines */

int lockrw_create(lockrw *psp,int f_shared) noex {
	int		rs = SR_FAULT ;
	if (psp) {
	    psp->magic = 0 ;
	    psp->readers = 0 ;
	    psp->writers = 0 ;
	    psp->waitwriters = 0 ;
	    psp->waitreaders = 0 ;
	    if ((rs = lockrw_ptminit(psp,f_shared)) >= 0) {
	        if ((rs = lockrw_ptcinit(psp,f_shared)) >= 0) {
		    psp->magic = LOCKRW_MAGIC ;
		}
	        if (rs < 0)
		    ptm_destroy(&psp->m) ;
	    } /* end if (PTM created) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lockrw_create) */

int lockrw_destroy(lockrw *psp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == LOCKRW_MAGIC) {
		rs = SR_OK ;
	        rs1 = ptc_destroy(&psp->c) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = ptm_destroy(&psp->m) ;
	        if (rs >= 0) rs = rs1 ;
	        psp->magic = 0 ;
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lockrw_destroy) */

int lockrw_rdlock(lockrw *psp,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == LOCKRW_MAGIC) {
	        TIMESPEC	ts{} ;
	        if (to >= 0) {
	            clock_gettime(CLOCK_REALTIME,&ts) ;
	            ts.tv_sec += to ;
	        }
	        if ((rs = ptm_lockto(&psp->m,to)) >= 0) {
	            psp->waitreaders += 1 ;
	            while ((rs >= 0) && lockrw_notready(psp,1)) {
		        if (to >= 0) {
	                    rs = ptc_timedwait(&psp->c,&psp->m,&ts) ;
		        } else {
	                    rs = ptc_wait(&psp->c,&psp->m) ;
		        }
	            } /* end while */
	            if (rs >= 0) {
		        n = psp->readers ;
		        psp->readers += 1 ;
	            }
	            psp->waitreaders -= 1 ;
	            rs1 = ptm_unlock(&psp->m) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ptm) */
	    } /* end if (was open) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (lockrw_rdlock) */

int lockrw_wrlock(lockrw *psp,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == LOCKRW_MAGIC) {
	        TIMESPEC	ts{} ;
	        if (to >= 0) {
	            clock_gettime(CLOCK_REALTIME,&ts) ;
	            ts.tv_sec += to ;
	        }
	        if ((rs = ptm_lockto(&psp->m,to)) >= 0) {
	            psp->waitwriters += 1 ;
	            while ((rs >= 0) && lockrw_notready(psp,0)) {
		        if (to >= 0) {
	                    rs = ptc_timedwait(&psp->c,&psp->m,&ts) ;
		        } else {
	                    rs = ptc_wait(&psp->c,&psp->m) ;
		        }
	            } /* end while */
	            if (rs >= 0) psp->writers += 1 ;
	            psp->waitwriters -= 1 ;
	            rs1 = ptm_unlock(&psp->m) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ptm) */
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lockrw_wrlock) */

int lockrw_unlock(lockrw *psp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == LOCKRW_MAGIC) {
	        if ((rs = ptm_lock(&psp->m)) >= 0) {
	            if (psp->readers > 0) psp->readers -= 1 ;
	            if (psp->writers > 0) psp->writers -= 1 ;
	            if ((psp->waitreaders > 0) || (psp->waitwriters > 0)) {
	                rs = ptc_broadcast(&psp->c) ;
	            }
	            rs1 = ptm_unlock(&psp->m) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex-lock) */
	    } /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lockrw_unlock) */

int lockrw_readers(lockrw *psp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		v = 0 ;
	if (psp) {
	    rs = SR_NOTOPEN ;
	    if (psp->magic == LOCKRW_MAGIC) {
	        if ((rs = ptm_lock(&psp->m)) >= 0) {
	            v = psp->readers ; /* this is really already atomic! */
	            rs1 = ptm_unlock(&psp->m) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex-lock) */
	    } /* end if (was open) */
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (lockrw_readers) */


/* private subroutines */

static int lockrw_ptminit(lockrw *psp,int f_shared) noex {
	ptma		a ;
	int		rs ;
	int		rs1 ;
	if ((rs = ptma_create(&a)) >= 0) {
	    bool	f_ptm = FALSE ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptma_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptm_create(&psp->m,&a) ;
		f_ptm = (rs >= 0) ;
	    }
	    rs1 = ptma_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptm) ptm_destroy(&psp->m) ;
	} /* end if (ptma) */
	return rs ;
}
/* end subroutine (lockrw_ptminit) */

static int lockrw_ptcinit(lockrw *psp,int f_shared) noex {
	PTCA		a ;
	int		rs ;
	int		rs1 ;
	if ((rs = ptca_create(&a)) >= 0) {
	    bool	f_ptc = FALSE ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptca_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptc_create(&psp->c,&a) ;
		f_ptc = (rs >= 0) ;
	    }
	    rs1 = ptca_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptc) ptc_destroy(&psp->c) ;
	} /* end if (ptca) */
	return rs ;
}
/* end subroutine (lockrw_ptcinit) */

static int lockrw_notready(lockrw *psp,int f_read) noex {
	bool	f_notready = (psp->writers > 0) ;
	if (f_read) {
	    f_notready = f_notready || (psp->waitwriters > 0) ;
	} else {
	    f_notready = f_notready || (psp->readers > 0) ;
	} /* end if */
	return f_notready ;
}
/* end subroutine (lockrw_notready) */

int lockrw_co::operator () (int to) noex {
	int	rs = SR_BUGCHECK ;
	switch (w) {
	case lockrwmem_create:
	    {
		cint	f = (to > 0) ? true : false ;
	        rs = lockrw_create(op,f) ;
	    }
	    break ;
	case lockrwmem_destroy:
	    rs = lockrw_destroy(op) ;
	    break ;
	case lockrwmem_rdlock:
	    rs = lockrw_rdlock(op,to) ;
	    break ;
	case lockrwmem_wrlock:
	    rs = lockrw_wrlock(op,to) ;
	    break ;
	case lockrwmem_unlock:
	    rs = lockrw_unlock(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (lockrw_co::operator) */


