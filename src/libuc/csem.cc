/* csem */
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
	semaphore. It uses the underlying POSIX mutex and
	condition-variables synchronization facility implementations.
	One would think that some OS (or even POSIX) would have
	given us a counting semaphore by now, but NO, that is not
	the case. Admittedly, there are no longer as many uses for
	a counting semaphore per se, now that the p-threads semaphore
	and p-threads condition variables are available. But we get
	by because the usual decrement amount on the semaphore count
	is usually only one (1), and that happens to be the only,
	but generally sufficient, decrement amount that p-threads
	semaphores allow for.

	Synopses:
	int csem_create(csem *psp,int f_shared,int count) noex
	int csem_destroy(csem *psp) noex
	int csem_decr(csem *psp,int c,int to) noex
	int csem_incr(csem *psp,int c) noex
	int csem_count(csem *psp) noex
	int csem_waiters(csem *psp) noex

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<time.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"csem.h"


/* local defines */


/* external subroutines */


/* forward references */

template<typename ... Args>
static inline int csem_ctor(csem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
	    op->count = 0 ;
	    op->waiters = 0 ;
	}
	return rs ;
}

template<typename ... Args>
static inline int csem_magic(csem *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CSEM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	csem_ptminit(csem *,int) noex ;
static int	csem_ptcinit(csem *,int) noex ;


/* local variables */


/* exported subroutines */

int csem_create(csem *psp,int f_shared,int count) noex {
	int		rs ;
	if ((rs = csem_ctor(psp)) >= 0) {
	    if (count < 1) count = 1 ;
	    psp->count = count ;
	    if ((rs = csem_ptminit(psp,f_shared)) >= 0) {
	        if ((rs = csem_ptcinit(psp,f_shared)) >= 0) {
		    psp->magic = CSEM_MAGIC ;
	        }
	        if (rs < 0)
		    ptm_destroy(&psp->m) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (csem_start) */

int csem_destroy(csem *psp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if ((rs = csem_magic(psp)) >= 0) {
	    {
		rs1 = ptc_destroy(&psp->c) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = ptm_destroy(&psp->m) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    psp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (csem_destroy) */

int csem_decr(csem *psp,int c,int to) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	if ((rs = csem_magic(psp)) >= 0) {
		if (c > 0) {
		    timespec	ts ;
	            if (to >= 0) {
	                clock_gettime(CLOCK_REALTIME,&ts) ;
	                ts.tv_sec += to ;
	            }
	            if ((rs = ptm_lockto(&psp->m,to)) >= 0) {
	                psp->waiters += 1 ;
	                while ((rs >= 0) && (psp->count < c)) {
		            if (to >= 0) {
	                        rs = ptc_timedwait(&psp->c,&psp->m,&ts) ;
		            } else {
	                        rs = ptc_wait(&psp->c,&psp->m) ;
		            }
	                } /* end while */
	                if (rs >= 0) {
		            ocount = psp->count ;
		            psp->count -= c ;
	                }
	                psp->waiters -= 1 ;
	                rs1 = ptm_unlock(&psp->m) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (ptm) */
		} else if (c == 0) {
		    rs = SR_OK ;
		} /* end if (valid decrement count) */
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
}
/* end subroutine (csem_decr) */

int csem_incr(csem *psp,int c) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	if ((rs = csem_magic(psp)) >= 0) {
		rs = SR_INVALID ;
		if (c > 0) {
	            if ((rs = ptm_lock(&psp->m)) >= 0) {
	                ocount = psp->count ;
	                psp->count += c ;
		        if ((ocount == 0) && (psp->waiters > 0)) {
	                    rs = ptc_signal(&psp->c) ;
	                }
	                rs1 = ptm_unlock(&psp->m) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex-lock) */
		} else if (c == 0) {
		    rs = SR_OK ;
		}
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
}
/* end subroutine (csem_incr) */

int csem_count(csem *psp) noex {
	int		rs ;
	int		rs1 ;
	int		ocount = 0 ;
	if ((rs = csem_magic(psp)) >= 0) {
	        if ((rs = ptm_lock(&psp->m)) >= 0) {
	            {
	                ocount = psp->count ;
	            }
	            rs1 = ptm_unlock(&psp->m) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? ocount : rs ;
}
/* end subroutine (csem_count) */

int csem_waiters(csem *psp) noex {
	int		rs ;
	int		rs1 ;
	int		waiters = 0 ;
	if ((rs = csem_magic(psp)) >= 0) {
	        if ((rs = ptm_lock(&psp->m)) >= 0) {
	            {
	                waiters = psp->waiters ;
	            }
	            rs1 = ptm_unlock(&psp->m) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex-lock) */
	} /* end if (magic) */
	return (rs >= 0) ? waiters : rs ;
}
/* end subroutine (csem_waiters) */


/* private subroutines */

static int csem_ptminit(csem *psp,int f_shared) noex {
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
/* end subroutine (csem_ptminit) */

static int csem_ptcinit(csem *psp,int f_shared) noex {
	ptca		a ;
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
	    if ((rs < 0) && f_ptc) {
		ptc_destroy(&psp->c) ;
	    }
	} /* end if (ptca) */
	return rs ;
}
/* end subroutine (csem_ptcinit) */


