/* csem HEADER (Counting Semaphore) */
/* encoding=ISO8859-1 */
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
	int csem_count(csem *op) noex
	int csem_waiters(csem *op) noex

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"csem.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int csem_ctor(csem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->cnt = 0 ;
	    op->nwaiting = 0 ;
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
/* end subroutine (csem_ctor) */

static int csem_dtor(csem *op) noex {
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
/* end subroutine (csem_dtor) */

template<typename ... Args>
static inline int csem_magic(csem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CSEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (csem_magic) */

static int	csem_ptminit(csem *,int) noex ;
static int	csem_ptcinit(csem *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int csem_create(csem *op,int f_shared,int count) noex {
	int		rs ;
	if ((rs = csem_ctor(op)) >= 0) {
	    op->cnt = (count > 0) ? count : 0 ;
	    if ((rs = csem_ptminit(op,f_shared)) >= 0) {
	        if ((rs = csem_ptcinit(op,f_shared)) >= 0) {
		    op->magic = CSEM_MAGIC ;
	        } /* end if (csem_ptcinit) */
	        if (rs < 0) {
		    ptm_destroy(op->mxp) ;
		}
	    } /* end if (csem_ptminit) */
	    if (rs < 0) {
		csem_dtor(op) ;
	    }
	} /* end if (csem_ctor) */
	return rs ;
}
/* end subroutine (csem_start) */

int csem_destroy(csem *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = csem_magic(op)) >= 0) {
	    {
		rs1 = ptc_destroy(op->cvp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = ptm_destroy(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = csem_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csem_destroy) */

int csem_decr(csem *op,int c,int to) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	if ((rs = csem_magic(op)) >= 0) {
            if (c > 0) {
                timespec    ts{} ;
                if (to >= 0) {
		    const clockid_t	cid = CLOCK_REALTIME ;
                    clock_gettime(cid,&ts) ;
                    ts.tv_sec += to ;
                }
                if ((rs = ptm_lockto(op->mxp,to)) >= 0) {
                    op->nwaiting += 1 ;
                    while ((rs >= 0) && (op->cnt < c)) {
                        if (to >= 0) {
                            rs = ptc_timedwait(op->cvp,op->mxp,&ts) ;
                        } else {
                            rs = ptc_wait(op->cvp,op->mxp) ;
                        }
                    } /* end while */
                    if (rs >= 0) {
                        ocount = op->cnt ;
                        op->cnt -= c ;
                    }
                    op->nwaiting -= 1 ;
                    rs1 = ptm_unlock(op->mxp) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (ptm) */
            } else if (c == 0) {
                rs = SR_OK ;
            } /* end if (valid decrement count) */
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
}
/* end subroutine (csem_decr) */

int csem_incr(csem *op,int c) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	if ((rs = csem_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (c > 0) {
	        if ((rs = ptm_lock(op->mxp)) >= 0) {
	            ocount = op->cnt ;
	            op->cnt += c ;
		    if ((ocount == 0) && (op->nwaiting > 0)) {
	                rs = ptc_signal(op->cvp) ;
	            }
	            rs1 = ptm_unlock(op->mxp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex-lock) */
	    } else if (c == 0) {
		rs = SR_OK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
}
/* end subroutine (csem_incr) */

int csem_count(csem *op) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	if ((rs = csem_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        {
	            ocount = op->cnt ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
}
/* end subroutine (csem_count) */

int csem_waiters(csem *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = csem_magic(op)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        {
	            c = op->nwaiting ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (csem_waiters) */


/* private subroutines */

static int csem_ptminit(csem *op,int f_shared) noex {
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
/* end subroutine (csem_ptminit) */

static int csem_ptcinit(csem *op,int f_shared) noex {
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
	    if ((rs < 0) && f_ptc) {
		ptc_destroy(op->cvp) ;
	    }
	} /* end if (ptca) */
	return rs ;
}
/* end subroutine (csem_ptcinit) */

int csem::create(int fshared,int sc) noex {
    	return csem_create(this,fshared,sc) ;
}

int csem::decr(int c,int to) noex {
    	return csem_decr(this,c,to) ;
}

int csem::incr(int c) noex {
    	return csem_incr(this,c) ;
}

void csem::dtor() noex {
	if (cint rs = destroy ; rs < 0) {
	    ulogerror("csem",rs,"fini-destroy") ;
	}
}

csem::operator int () noex {
	int		rs = SR_NOTOPEN ;
	if (mxp && cvp) {
	    rs = cnt ;
	}
	return rs ;
}

csem_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
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
}
/* end method (csem_co::operator) */


