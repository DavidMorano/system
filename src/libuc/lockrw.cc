/* lockrw SUPPORT */
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<ctime>
#include	<new>
#include	<usystem.h>
#include	<localmisc.h>

#include	"lockrw.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* forward references */

template<typename ... Args>
static inline int lockrw_ctor(lockrw *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->readers = 0 ;
	    op->writers = 0 ;
	    op->waitwriters = 0 ;
	    op->waitreaders = 0 ;
	    op->cvp = nullptr ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->cvp = new(nothrow) ptc) != np) {
		    rs = SR_OK ;
	        } /* end if (new-ptc) */
	 	if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
		}
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lockrw_ctor) */

static int lockrw_dtor(lockrw *op) noex {
	int		rs = SR_OK ;
	if (op->cvp) {
	    delete op->cvp ;
	    op->cvp = nullptr ;
	}
	if (op->mxp) {
	    delete op->mxp ;
	    op->mxp = nullptr ;
	}
	return rs ;
}
/* end subroutine (lockrw_dtor) */

template<typename ... Args>
static inline int lockrw_magic(lockrw *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LOCKRW_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (lockrw_magic) */

static int	lockrw_ptminit(lockrw *,int) noex ;
static int	lockrw_ptcinit(lockrw *,int) noex ;
static int	lockrw_notready(lockrw *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int lockrw_create(lockrw *op,int f_shared) noex {
	int		rs ;
	if ((rs = lockrw_ctor(op)) >= 0) {
	    if ((rs = lockrw_ptminit(op,f_shared)) >= 0) {
	        if ((rs = lockrw_ptcinit(op,f_shared)) >= 0) {
		    op->magic = LOCKRW_MAGIC ;
		}
	        if (rs < 0) {
		    ptm_destroy(op->mxp) ;
		}
	    } /* end if (PTM created) */
	    if (rs < 0) {
		lockrw_dtor(op) ;
	    }
	} /* end if (lockrw_ctor) */
	return rs ;
}
/* end subroutine (lockrw_create) */

int lockrw_destroy(lockrw *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lockrw_magic(op)) >= 0) {
	    {
	        rs1 = ptc_destroy(op->cvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = lockrw_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lockrw_destroy) */

int lockrw_rdlock(lockrw *op,int to) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = lockrw_magic(op)) >= 0) {
	        TIMESPEC	ts{} ;
	        if (to >= 0) {
	            clock_gettime(CLOCK_REALTIME,&ts) ;
	            ts.tv_sec += to ;
	        }
	        if ((rs = ptm_lockto(op->mxp,to)) >= 0) {
	            op->waitreaders += 1 ;
	            while ((rs >= 0) && lockrw_notready(op,1)) {
		        if (to >= 0) {
	                    rs = ptc_timedwait(op->cvp,op->mxp,&ts) ;
		        } else {
	                    rs = ptc_wait(op->cvp,op->mxp) ;
		        }
	            } /* end while */
	            if (rs >= 0) {
		        n = op->readers ;
		        op->readers += 1 ;
	            }
	            op->waitreaders -= 1 ;
	            rs1 = ptm_unlock(op->mxp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (lockrw_rdlock) */

int lockrw_wrlock(lockrw *op,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lockrw_magic(op)) >= 0) {
	        TIMESPEC	ts{} ;
	        if (to >= 0) {
	            clock_gettime(CLOCK_REALTIME,&ts) ;
	            ts.tv_sec += to ;
	        }
	        if ((rs = ptm_lockto(op->mxp,to)) >= 0) {
	            op->waitwriters += 1 ;
	            while ((rs >= 0) && lockrw_notready(op,0)) {
		        if (to >= 0) {
	                    rs = ptc_timedwait(op->cvp,op->mxp,&ts) ;
		        } else {
	                    rs = ptc_wait(op->cvp,op->mxp) ;
		        }
	            } /* end while */
	            if (rs >= 0) op->writers += 1 ;
	            op->waitwriters -= 1 ;
	            rs1 = ptm_unlock(op->mxp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ptm) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lockrw_wrlock) */

int lockrw_unlock(lockrw *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lockrw_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (op->readers > 0) op->readers -= 1 ;
	        if (op->writers > 0) op->writers -= 1 ;
	        if ((op->waitreaders > 0) || (op->waitwriters > 0)) {
	            rs = ptc_broadcast(op->cvp) ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-lock) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lockrw_unlock) */

int lockrw_readers(lockrw *op) noex {
	int		rs ;
	int		rs1 ;
	int		v = 0 ;
	if ((rs = lockrw_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		{
	            v = op->readers ; /* this is really already atomic! */
		}
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (lockrw_readers) */


/* private subroutines */

static int lockrw_ptminit(lockrw *op,int f_shared) noex {
	ptma		a ;
	int		rs ;
	int		rs1 ;
	if ((rs = ptma_create(&a)) >= 0) {
	    bool	f_ptm = false ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptma_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptm_create(op->mxp,&a) ;
		f_ptm = (rs >= 0) ;
	    }
	    rs1 = ptma_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptm) ptm_destroy(op->mxp) ;
	} /* end if (ptma) */
	return rs ;
}
/* end subroutine (lockrw_ptminit) */

static int lockrw_ptcinit(lockrw *op,int f_shared) noex {
	ptca		a ;
	int		rs ;
	int		rs1 ;
	if ((rs = ptca_create(&a)) >= 0) {
	    bool	f_ptc = false ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptca_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = ptc_create(op->cvp,&a) ;
		f_ptc = (rs >= 0) ;
	    }
	    rs1 = ptca_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptc) ptc_destroy(op->cvp) ;
	} /* end if (ptca) */
	return rs ;
}
/* end subroutine (lockrw_ptcinit) */

static int lockrw_notready(lockrw *op,int f_read) noex {
	bool	f_notready = (op->writers > 0) ;
	if (f_read) {
	    f_notready = f_notready || (op->waitwriters > 0) ;
	} else {
	    f_notready = f_notready || (op->readers > 0) ;
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


