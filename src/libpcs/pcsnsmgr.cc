/* pcsnsmgr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PCS Name-Server Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pcsnsmgr

	Description:
	This module manages the in-memory cache for the PCS Name-Server
	shared-object module.

	Notes:

	Q. The old age question: do these (public) subroutines need to be
	multi-thread-safe?
	A. What do you think?

	Q. Why cannot we just use a POSIX® mutex-lock around the
	guts of the public subroutines?
	A. Because those "guts" might do some complex operating-system-like
	things that would lead to a deadlock (because perhaps there
	is a |uc_fork(3uc)| in there, for example)!  It is *much*
	better to be safe than sorry.

	Q. Your (our) little "capture" mutex-lock implementation:
	why did you not just use a |lockrw(3dam)| object lock in
	exclusive (write-lock) mode -- because that is pretty much
	exactly what we have there?
	A. Oh, I do not know. I guess that originally I needed a
	more complicated lock of some sort (although that did not
	turn out to be the case), so going custom was what was
	needed.  As it is, a |lockrw(3dam)| would have been perfectly
	adequate.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>
#include	<usystem.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"pcsnsmgr.h"
#include	"pcsnsrecs.h"


/* local defines */

#define	PCSNSMGR	struct pcsnsmgr_head


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		pcsnsmgr_init() noex ;
    int		pcsnsmgr_fini() noex ;
}


/* external variables */


/* local structures */

struct pcsnsmgr_head {
	ptm		m ;		/* data mutex */
	ptc		c ;		/* condition variable */
	PCSNSRECS	*recs ;		/* records (allocated) */
	int		max ;
	int		ttl ;
	volatile int	waiters ;
	volatile int	f_capture ;	/* capture flag */
	volatile int	f_init ;	/* race-condition, blah, blah */
	volatile int	f_initdone ;
} ;


/* forward references */

static int	pcsnsmgr_begin(PCSNSMGR *) noex ;
static int	pcsnsmgr_end(PCSNSMGR *) noex ;
static int	pcsnsmgr_capbegin(PCSNSMGR *,int) noex ;
static int	pcsnsmgr_capend(PCSNSMGR *) noex ;

extern "C" {
    static void	pcsnsmgr_atforkbefore() noex ;
    static void	pcsnsmgr_atforkafter() noex ;
    static void	pcsnsmgr_exit() noex ;
}


/* local variables */

static PCSNSMGR		pcsnsmgr_data ;


/* exported variables */


/* exported subroutines */

int pcsnsmgr_init() noex {
	PCSNSMGR	*uip = &pcsnsmgr_data ;
	int		rs = SR_OK ;
	int		f = false ;
	if (! uip->f_init) {
	    uip->f_init = true ;
	    if ((rs = ptm_create(&uip->m,nullptr)) >= 0) {
	        if ((rs = ptc_create(&uip->c,nullptr)) >= 0) {
	    	    void_f	b = pcsnsmgr_atforkbefore ;
	    	    void_f	a = pcsnsmgr_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(pcsnsmgr_exit)) >= 0) {
	                    uip->f_initdone = true ;
			    f = true ;
	                }
	                if (rs < 0)
	                    uc_atforkexpunge(b,a,a) ;
	            } /* end if (uc_atfork) */
	            if (rs < 0)
	                ptc_destroy(&uip->c) ;
	        } /* end if (ptc_create) */
	        if (rs < 0)
	            ptm_destroy(&uip->m) ;
	    } /* end if (ptm_create) */
	    if (rs < 0)
	        uip->f_init = false ;
	} else {
	    while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
		rs = msleep(1) ;
		if (rs == SR_INTR) rs = SR_OK ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pcsnsmgr_init) */

int pcsnsmgr_fini() noex {
	PCSNSMGR	*uip = &pcsnsmgr_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone) {
	    uip->f_initdone = false ;
	    if (uip->recs) {
	        rs1 = pcsnsmgr_end(uip) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = pcsnsmgr_atforkbefore ;
	        void_f	a = pcsnsmgr_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptc_destroy(&uip->c) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&uip->m) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_initdone = false ;
	    uip->f_init = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (pcsnsmgr_fini) */

int pcsnsmgr_set(cchar *vbuf,int vlen,cchar *un,int w,int ttl) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;

	if (vbuf == nullptr) return SR_FAULT ;
	if (un == nullptr) return SR_FAULT ;

	if (un[0] == '\0') return SR_INVALID ;

	if ((rs = pcsnsmgr_init()) >= 0) {
	    PCSNSMGR	*uip = &pcsnsmgr_data ;
	    if ((rs = pcsnsmgr_capbegin(uip,-1)) >= 0) {

	        if (uip->recs == nullptr) rs = pcsnsmgr_begin(uip) ;

	        if (rs >= 0) {
	            PCSNSRECS	*recsp = (PCSNSRECS *) uip->recs ;
	            rs = pcsnsrecs_store(recsp,vbuf,vlen,un,w,ttl) ;
		    len = rs ;
	        } /* end if (lookup) */

	        rs1 = pcsnsmgr_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture-exclusion) */
	} /* end if (init) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pcsnsmgr_set) */

int pcsnsmgr_get(char *rbuf,int rlen,cchar *un,int w) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;

	if (rbuf == nullptr) return SR_FAULT ;
	if (un == nullptr) return SR_FAULT ;

	rbuf[0] = '\0' ;
	if ((rs = pcsnsmgr_init()) >= 0) {
	    PCSNSMGR	*uip = &pcsnsmgr_data ;
	    if ((rs = pcsnsmgr_capbegin(uip,-1)) >= 0) {

	        if (uip->recs != nullptr) {
	            PCSNSRECS	*recsp = (PCSNSRECS *) uip->recs ;
	            if ((rs = pcsnsrecs_lookup(recsp,rbuf,rlen,un,w)) >= 0) {
	                len = rs ;
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
	        } /* end if (active) */

	        rs1 = pcsnsmgr_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture-exclusion) */
	} /* end if (init) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pcsnsmgr_get) */

int pcsnsmgr_invalidate(cchar *un,int w) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;

	if (un == nullptr) return SR_FAULT ;

	if ((rs = pcsnsmgr_init()) >= 0) {
	    PCSNSMGR	*uip = &pcsnsmgr_data ;
	    if ((rs = pcsnsmgr_capbegin(uip,-1)) >= 0) {

	        if (uip->recs != nullptr) {
	            PCSNSRECS	*recsp = (PCSNSRECS *) uip->recs ;
	            if ((rs = pcsnsrecs_invalidate(recsp,un,w)) >= 0) {
	                len = rs ;
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
	        } /* end if (active) */

	        rs1 = pcsnsmgr_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture-exclusion) */
	} /* end if (init) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pcsnsmgr_invalidate) */

int pcsnsmgr_stats(PCSNSMGR_STATS *usp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

	if (usp == nullptr) return SR_FAULT ;

	memset(usp,0,sizeof(PCSNSMGR_STATS)) ;

	if ((rs = pcsnsmgr_init()) >= 0) {
	    PCSNSMGR	*uip = &pcsnsmgr_data ;
	    if ((rs = pcsnsmgr_capbegin(uip,-1)) >= 0) {

	        if (uip->recs == nullptr) rs = pcsnsmgr_begin(uip) ;

	        if (rs >= 0) {
	            PCSNSRECS		*recsp = (PCSNSRECS *) uip->recs ;
	            PCSNSRECS_ST	s ;
	            if ((rs = pcsnsrecs_stats(recsp,&s)) >= 0) {
	                usp->max = uip->max ;
	                usp->ttl = uip->ttl ;
	                usp->nent = s.nentries ;
	                usp->acc = s.total ;
	                usp->phit = s.phits ;
	                usp->nhit = s.nhits ;
	                usp->pmis = s.pmisses ;
	                usp->nmis = s.nmisses ;
	                n = s.nentries ;
	            } /* end if (pcsnsrecs-stats) */
	        } /* end if */

	        rs1 = pcsnsmgr_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture-exclusion) */
	} /* end if (init) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (pcsnsmgr_stats) */


/* local subroutines (or "private"?) */

static int pcsnsmgr_capbegin(PCSNSMGR *uip,int to) noex {
	int		rs ;
	int		rs1 ;

	if ((rs = ptm_lockto(&uip->m,to)) >= 0) {
	    uip->waiters += 1 ;

	    while ((rs >= 0) && uip->f_capture) { /* busy */
	        rs = ptc_waiter(&uip->c,&uip->m,to) ;
	    } /* end while */

	    if (rs >= 0) {
	        uip->f_capture = true ;
	    }

	    uip->waiters -= 1 ;
	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (pcsnsmgr_capbegin) */

static int pcsnsmgr_capend(PCSNSMGR *uip) noex {
	int		rs ;
	int		rs1 ;

	if ((rs = ptm_lock(&uip->m)) >= 0) {

	    uip->f_capture = false ;
	    if (uip->waiters > 0) {
	        rs = ptc_signal(&uip->c) ;
	    }

	    rs1 = ptm_unlock(&uip->m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (pcsnsmgr_capend) */

static void pcsnsmgr_atforkbefore() noex {
	PCSNSMGR		*uip = &pcsnsmgr_data ;
	ptm_lock(&uip->m) ;
}
/* end subroutine (pcsnsmgr_atforkbefore) */

static void pcsnsmgr_atforkafter() noex {
	PCSNSMGR		*uip = &pcsnsmgr_data ;
	ptm_unlock(&uip->m) ;
}
/* end subroutine (pcsnsmgr_atforkafter) */

static void pcsnsmgr_exit() noex {
	cint		rs = pcsnsmgr_fini() ;
	if (rs < 0) {
	    ulogerror("pcsnsmgr",rs,"exit-fini") ;
	}
}
/* end subroutine (pcsnsmgr_exit) */

static int pcsnsmgr_begin(PCSNSMGR *uip) noex {
	int		rs = SR_OK ;

	if (uip->recs == nullptr) {
	    cint	size = sizeof(PCSNSRECS) ;
	    void	*p ;
	    if ((rs = lm_mall(size,&p)) >= 0) {
	        cint		max = PCSNSMGR_MAX ;
	        cint		ttl = PCSNSMGR_TTL ;
	        PCSNSRECS	*recsp = (PCSNSRECS *) p ;
	        if ((rs = pcsnsrecs_start(recsp,max,ttl)) >= 0) {
	            uip->recs = recsp ;
	            uip->max = max ;
	            uip->ttl = ttl ;
		}
	        if (rs < 0)
	            lm_free(p) ;
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */

	return rs ;
}
/* end subroutine (pcsnsmgr_begin) */

static int pcsnsmgr_end(PCSNSMGR *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (uip->recs != nullptr) {
	    PCSNSRECS	*recsp = (PCSNSRECS *) uip->recs ;
	    rs1 = pcsnsrecs_finish(recsp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = lm_free(uip->recs) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->recs = nullptr ;
	}

	return rs ;
}
/* end subroutine (pcsnsmgr_end) */


