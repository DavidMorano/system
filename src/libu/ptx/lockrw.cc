/* lockrw SUPPORT */
/* charset=ISO8859-1 */
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

  	Object:
	lockrw

	Description:
	The Solaris® implmentation of the POSIX® read-write lock
	is BROKEN!  We can all thank the "smart" Solaris® developers
	for that.  How did Solaris® manage to create a borken
	implementation?  Firstly, they thought that they were really
	smart people.  Secondly, from their own literature, they
	created a read-write lock that was especially "optimized,"
	whatever that means. They forgot that it was more important
	to create a *working* version of a piece of code, than a
	fast *broken* version of that code.  This module provides
	a working (and minimally smart) read-write lock.

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
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"lockrw.h"


/* local defines */


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* forward references */

template<typename ... Args>
local inline int lockrw_ctor(lockrw *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->readers = 0 ;
	    op->writers = 0 ;
	    op->waitwriters = 0 ;
	    op->waitreaders = 0 ;
	    op->cvp = nullptr ;
	    if ((op->mxp = new(nothrow) ptm) != np) ylikely {
	        if ((op->cvp = new(nothrow) ptc) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-ptc) */
	 	if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
		}
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lockrw_ctor) */

local int lockrw_dtor(lockrw *op) noex {
	int		rs = SR_OK ;
	if (op->cvp) ylikely {
	    delete op->cvp ;
	    op->cvp = nullptr ;
	}
	if (op->mxp) ylikely {
	    delete op->mxp ;
	    op->mxp = nullptr ;
	}
	return rs ;
} /* end subroutine (lockrw_dtor) */

template<typename ... Args>
local inline int lockrw_magic(lockrw *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == LOCKRW_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (lockrw_magic) */

local int	lockrw_ptminit(lockrw *,int) noex ;
local int	lockrw_ptcinit(lockrw *,int) noex ;
local int	lockrw_notready(lockrw *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int lockrw_create(lockrw *op,int f_shared) noex {
	int		rs ;
	if ((rs = lockrw_ctor(op)) >= 0) ylikely {
	    if ((rs = lockrw_ptminit(op,f_shared)) >= 0) ylikely {
	        if ((rs = lockrw_ptcinit(op,f_shared)) >= 0) ylikely {
		    op->magic = LOCKRW_MAGIC ;
		}
	        if (rs < 0) {
		    ptm *mxp = op->mxp ;
		    mxp->destroy() ;
		} /* end if (error) */
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
	if ((rs = lockrw_magic(op)) >= 0) ylikely {
	    if (op->cvp) ylikely {
		ptc *cvp = op->cvp ;
	        rs1 = cvp->destroy ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) ylikely {
		ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
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
	if ((rs = lockrw_magic(op)) >= 0) ylikely {
	    TIMESPEC	ts{} ;
	    ptm *mxp = op->mxp ;
	    ptc *cvp = op->cvp ;
	    if (to >= 0) {
		const clockid_t	cid = CLOCK_REALTIME ;
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    } /* end if (handle possible time-out) */
	    if ((rs = mxp->lockbegin(to)) >= 0) ylikely {
	        op->waitreaders += 1 ;
	        while ((rs >= 0) && lockrw_notready(op,1)) {
		    if (to >= 0) {
	                rs = cvp->waiter(op->mxp,&ts) ;
		    } else {
	                rs = cvp->wait(op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    n = op->readers ;
		    op->readers += 1 ;
	        }
	        op->waitreaders -= 1 ;
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (lockrw_rdlock) */

int lockrw_wrlock(lockrw *op,int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lockrw_magic(op)) >= 0) ylikely {
	    TIMESPEC	ts{} ;
	    ptm *mxp = op->mxp ;
	    ptc *cvp = op->cvp ;
	    if (to >= 0) {
		const clockid_t	cid = CLOCK_REALTIME ;
	        clock_gettime(cid,&ts) ;
	        ts.tv_sec += to ;
	    } /* end if (time-out requested) */
	    if ((rs = mxp->lockbegin(to)) >= 0) ylikely {
	        op->waitwriters += 1 ;
	        while ((rs >= 0) && lockrw_notready(op,0)) {
		    if (to >= 0) {
	                rs = cvp->waiter(op->mxp,&ts) ;
		    } else {
	                rs = cvp->wait(op->mxp) ;
		    }
	        } /* end while */
	        if (rs >= 0) {
		    op->writers += 1 ;
		}
	        op->waitwriters -= 1 ;
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lockrw_wrlock) */

int lockrw_unlock(lockrw *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lockrw_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    ptc *cvp = op->cvp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        if (op->readers > 0) op->readers -= 1 ;
	        if (op->writers > 0) op->writers -= 1 ;
	        if ((op->waitreaders > 0) || (op->waitwriters > 0)) {
	            rs = cvp->broadcast ;
	        }
	        rs1 = mxp->lockend ;
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
	if ((rs = lockrw_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
		{
	            v = op->readers ; /* this is really already atomic! */
		}
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (lockrw_readers) */


/* private subroutines */

local int lockrw_ptminit(lockrw *op,int f_shared) noex {
	int		rs ;
	int		rs1 ;
	if (ptma a ; (rs = ptma_create(&a)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    bool	f_ptm = false ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptma_setpshared(&a,v) ;
	    }
	    if (rs >= 0) {
	        rs = mxp->create(&a) ;
		f_ptm = (rs >= 0) ;
	    }
	    rs1 = ptma_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptm) {
		mxp->destroy() ;
	    }
	} /* end if (ptma) */
	return rs ;
}
/* end subroutine (lockrw_ptminit) */

local int lockrw_ptcinit(lockrw *op,int f_shared) noex {
	int		rs ;
	int		rs1 ;
	if (ptca a ; (rs = ptca_create(&a)) >= 0) ylikely {
	    ptc *cvp = op->cvp ;
	    bool	f_ptc = false ;
	    if (f_shared) {
		cint	v = PTHREAD_PROCESS_SHARED ;
		rs = ptca_setpshared(&a,v) ;
	    }
	    if (rs >= 0) ylikely {
	        rs = cvp->create(&a) ;
		f_ptc = (rs >= 0) ;
	    }
	    rs1 = ptca_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptc) {
		cvp->destroy() ;
	    }
	} /* end if (ptca) */
	return rs ;
}
/* end subroutine (lockrw_ptcinit) */

local int lockrw_notready(lockrw *op,int f_read) noex {
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
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case lockrwmem_create:
	        {
		    cint	f = (to > 0) ;
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
	} /* end if (non-null) */
	return rs ;
}
/* end method (lockrw_co::operator) */

void lockrw::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("lockrw",rs,"fini-finish") ;
	}
} /* end method (lockrw::dtor) */


