/* csem SUPPORT (Counting Semaphore) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Counting-Semaphore (CSEM) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	csem_create
	csem_destroy
	csem_decr
	csem_incr
	csem_post
	csem_count
	csem_waiters

	Description:
	This module provides an interlocked (atomic) counting
	semaphore.  It uses the underlying POSIX® mutex and
	condition-variables synchronization facility implementations.
	One would think that some OS (or even POSIX) would have
	given us a counting semaphore by now, but NO, that is not
	the case.  Admittedly, there are no longer as many uses for
	a counting semaphore per se, now that the p-threads semaphore
	and p-threads condition variables are available.  But we get
	by because the usual decrement amount on the semaphore count
	is usually only one (1), and that happens to be the only,
	but generally sufficient, decrement amount that p-threads
	semaphores allow for.

	Synopses:
	int csem_create(csem *op,int f_shared,int count) noex
	int csem_destroy(csem *op) noex
	int csem_decr(csem *op,int c,int to) noex
	int csem_incr(csem *op,int c) noex
	int csem_post(csem *op) noex
	int csem_count(csem *op) noex
	int csem_waiters(csem *op) noex

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD |clock_{x}(3c++)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cassert>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"csem.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int csem_ctor(csem *op,Args ... args) noex {
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->magval = 0 ;
	    op->cnt = 0 ;
	    op->nwaiting = 0 ;
	    if ((op->mxp = new(nt) ptm) != np) ylikely {
	        if ((op->cvp = new(nt) ptc) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-ptc) */
	 	if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
		}
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (csem_ctor) */

local int csem_dtor(csem *op) noex {
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
} /* end subroutine (csem_dtor) */

template<typename ... Args>
local inline int csem_magic(csem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CSEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (csem_magic) */

local int	csem_ptminit(csem *,int) noex ;
local int	csem_ptcinit(csem *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int csem_create(csem *op,int f_shared,int count) noex {
	int		rs ;
	assert(op) ;
	if ((rs = csem_ctor(op)) >= 0) ylikely {
	    op->cnt = (count > 0) ? count : 0 ;
	    if ((rs = csem_ptminit(op,f_shared)) >= 0) ylikely {
		ptm *mxp = op->mxp ;
	        if ((rs = csem_ptcinit(op,f_shared)) >= 0) ylikely {
		    op->magval = CSEM_MAGIC ;
	        } /* end if (csem_ptcinit) */
	        if (rs < 0) {
		    mxp->destroy() ;
		} /* end if (error) */
	    } /* end if (csem_ptminit) */
	    if (rs < 0) {
		csem_dtor(op) ;
	    } /* end if (error) */
	} /* end if (csem_ctor) */
	return rs ;
} /* end subroutine (csem_start) */

int csem_destroy(csem *op) noex {
	int		rs ;
	int		rs1 ;
	assert(op) ;
	if ((rs = csem_magic(op)) >= 0) ylikely {
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
		rs1 = csem_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (csem_destroy) */

int csem_decr(csem *op,int c,int to) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	assert(op) ;
	if ((rs = csem_magic(op)) >= 0) ylikely {
            if (c > 0) {
                timespec    ts{} ; /* used-afterwards */
		ptm *mxp = op->mxp ;
		ptc *cvp = op->cvp ;
                if (to >= 0) {
		    const clockid_t	cid = CLOCK_REALTIME ;
                    clock_gettime(cid,&ts) ;
                    ts.tv_sec += to ;
                } /* end if (time-out requested) */
                if ((rs = mxp->lockbegin(to)) >= 0) ylikely {
		    {
                        op->nwaiting += 1 ;
                        while ((rs >= 0) && (op->cnt < c)) {
                            if (to >= 0) {
                                rs = cvp->waiter(op->mxp,&ts) ;
                            } else {
                                rs = cvp->wait(op->mxp) ;
                            }
                        } /* end while */
                        if (rs >= 0) ylikely {
                            ocount = op->cnt ;
                            op->cnt -= c ;
                        } /* end if (ok) */
                        op->nwaiting -= 1 ;
		    } /* end block */
                    rs1 = mxp->lockend ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (ptm) */
            } else if (c == 0) {
                rs = SR_OK ;
            } /* end if (valid decrement count) */
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
} /* end subroutine (csem_decr) */

int csem_incr(csem *op,int c) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	assert(op) ;
	if ((rs = csem_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    ptc *cvp = op->cvp ;
	    rs = SR_INVALID ;
	    if (c > 0) {
	        if ((rs = mxp->lockbegin) >= 0) ylikely {
		    {
	                ocount = op->cnt ;
	                op->cnt += c ;
		        if ((ocount == 0) && (op->nwaiting > 0)) {
	                    rs = cvp->signal ;
	                }
		    } /* end block */
	            rs1 = mxp->lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex-lock) */
	    } else if (c == 0) {
		rs = SR_OK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
} /* end subroutine (csem_incr) */

int csem_post(csem *op) noex {
	assert(op) ;
    	return csem_incr(op,1) ;
} /* end subroutine */

int csem_count(csem *op) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	assert(op) ;
	if ((rs = csem_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            ocount = op->cnt ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
} /* end subroutine (csem_count) */

int csem_waiters(csem *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	assert(op) ;
	if ((rs = csem_magic(op)) >= 0) ylikely {
	    ptm *mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) ylikely {
	        {
	            c = op->nwaiting ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (csem_waiters) */


/* private subroutines */

local int csem_ptminit(csem *op,int f_shared) noex {
	ptma		a ;
	int		rs ;
	int		rs1 ;
	if ((rs = ptma_create(&a)) >= 0) ylikely {
	    ptm		*mxp = op->mxp ;
	    bool	f_ptm = false ;
	    {
	        if (f_shared) {
		    cint	v = PTHREAD_PROCESS_SHARED ;
		    rs = ptma_setpshared(&a,v) ;
	        }
	        if (rs >= 0) ylikely {
	            rs = mxp->create(&a) ;
		    f_ptm = (rs >= 0) ;
	        }
	    } /* end block */
	    rs1 = ptma_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptm) {
		mxp->destroy() ;
	    } /* end if (error) */
	} /* end if (ptma) */
	return rs ;
} /* end subroutine (csem_ptminit) */

local int csem_ptcinit(csem *op,int f_shared) noex {
	int		rs ;
	int		rs1 ;
	if (ptca a ; (rs = ptca_create(&a)) >= 0) ylikely {
	    ptc		*cvp = op->cvp ;
	    bool	f_ptc = false ;
	    {
	        if (f_shared) {
		    cint	v = PTHREAD_PROCESS_SHARED ;
		    rs = ptca_setpshared(&a,v) ;
	        }
	        if (rs >= 0) ylikely {
	            rs = cvp->create(&a) ;
		    f_ptc = (rs >= 0) ;
	        } /* end if (ok) */
	    } /* end block */
	    rs1 = ptca_destroy(&a) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_ptc) {
		cvp->destroy() ;
	    } /* end if (error) */
	} /* end if (ptca) */
	return rs ;
} /* end subroutine (csem_ptcinit) */

int csem::create(int fshared,int sc) noex {
    	return csem_create(this,fshared,sc) ;
} /* end method */

int csem::decr(int c,int to) noex {
    	return csem_decr(this,c,to) ;
} /* end method */

void csem::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("csem",rs,"fini-destroy") ;
	}
} /* end method (csem::dtor) */

csem::operator int () noex {
	int		rs = SR_NOTOPEN ;
	if (mxp && cvp) {
	    rs = cnt ;
	}
	return rs ;
} /* end method (csem::operator) */

int csem_co::operator () (int c) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case csemmem_incr:
	        rs = csem_incr(op,c) ;
	        break ;
	    case csemmem_post:
	        rs = csem_incr(op,1) ;
	        break ;
	    case csemmem_count:
	        rs = csem_count(op) ;
	        break ;
	    case csemmem_waiters:
	        rs = csem_waiters(op) ;
	        break ;
	    case csemmem_destroy:
	        rs = csem_destroy(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (csem_co::operator) */


