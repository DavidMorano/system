/* ucygetpw SUPPORT */
/* lang=C++20 */

/* get UNIX® password entries (w/ cache) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This module serves as a per-process cache for UNIX®
	password entries.

	Since we are basically dealing with global data, we need
	to make the establishment of it multi-thread safe.  We also
	want fork safety.  Yes, we want everything, including cleanup
	on module unloading (since, yes, we could all be inside a
	loadable and unloadble module!).  For these purposes we
	employ the basic (and not so basic) means of accomplishing
	this.  See the code for our various machinations.

	Notes:

	Q. The old age question: do these (public) subroutines need
	to be multi-thread-safe?
	A. What do you think?

	Q. Why cannot we just use a POSIXÂ® mutex-lock around the
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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pwd.h>
#include	<csignal>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<upwcache.h>
#include	<localmisc.h>

#include	"ucygetpw.h"


/* local defines */

#define	ucygetpw		struct ucygetpw_head


/* local typedefs */


/* external subroutines */


/* local structures */

struct ucygetpw_head {
	ptm		m ;		/* data mutex */
	ptc		c ;		/* condition variable */
	ucpwcache	*pwc ;		/* PW cache (allocated) */
	int		max ;
	int		ttl ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		f_capture ;	/* capture flag */
	aflag		fvoid ;
	aflag		waiters ;
} ;


/* forward references */

static int	ucygetpw_begin(ucygetpw *) noex ;
static int	ucygetpw_end(ucygetpw *) noex ;
static int	ucygetpw_capbegin(ucygetpw *,int) noex ;
static int	ucygetpw_capend(ucygetpw *) noex ;

static void	ucygetpw_atforkbefore() noex ;
static void	ucygetpw_atforkafter() noex ;


/* local variables */

static ucygetpw		ucygetpw_data ;


/* exported subroutines */

int ucygetpw_init() noex {
	ucygetpw		*uip = &ucygetpw_data ;
	int		rs = SR_OK ;
	int		f = false ;
	if (! uip->finit.testandset) {			/* <- the money shot */
	    if ((rs = ptm_create(&uip->m,NULL)) >= 0) {
	        if ((rs = ptc_create(&uip->c,NULL)) >= 0) {
	    	    void	(*b)() = ucygetpw_atforkbefore ;
	    	    void	(*a)() = ucygetpw_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(ucygetpw_fini)) >= 0) {
	                    uip->finitdone = true ;
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
	        uip->finit = false ;
	} else {
	    while ((rs >= 0) && uip->finit && (! uip->finitdone)) {
	        rs = msleep(1) ;
	        if (rs == SR_INTR) rs = SR_OK ;
	    }
	    if ((rs >= 0) && (! uip->finit)) rs = SR_LOCKLOST ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ucygetpw_init) */

void ucygetpw_fini() noex {
	ucygetpw		*uip = &ucygetpw_data ;
	if (uip->finitdone) {
	    uip->finitdone = false ;
	    {
	        if (uip->pwc != NULL) {
	            ucygetpw_end(uip) ;
	        }
	    }
	    {
	        void	(*b)() = ucygetpw_atforkbefore ;
	        void	(*a)() = ucygetpw_atforkafter ;
	        uc_atforkexpunge(b,a,a) ;
	    }
	    ptc_destroy(&uip->c) ;
	    ptm_destroy(&uip->m) ;
	    memset(uip,0,sizeof(ucygetpw)) ;
	} /* end if (was initialized) */
}
/* end subroutine (ucygetpw_fini) */

int ucygetpw_name(passwd *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pwp && pwbuf && un) {
	memset(pwp,0,sizeof(struct passwd)) ;
	if ((rs = ucygetpw_init()) >= 0) {
	    ucygetpw	*uip = &ucygetpw_data ;
	    if ((rs = ucygetpw_capbegin(uip,-1)) >= 0) {
	        if (uip->pwc == NULL) rs = ucygetpw_begin(uip) ;
	        if (rs >= 0) {
	            ucpwcache	*pwcp = (ucpwcache *) uip->pwc ;
	            rs = upwcache_lookup(pwcp,pwp,pwbuf,pwlen,un) ;
	            len = rs ;
	        } /* end if (lookup) */
	        rs1 = ucygetpw_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ucygetpw-cap) */
	} /* end if (ucygetpw_init) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucygetpw_name) */

int ucygetpw_uid(passwd *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pwp && pwbuf) {
	memset(pwp,0,sizeof(struct passwd)) ;
	if ((rs = ucygetpw_init()) >= 0) {
	    ucygetpw	*uip = &ucygetpw_data ;
	    if ((rs = ucygetpw_capbegin(uip,-1)) >= 0) {
	        if (uip->pwc == NULL) rs = ucygetpw_begin(uip) ;
	        if (rs >= 0) {
	            ucpwcache	*pwcp = (ucpwcache *) uip->pwc ;
	            rs = upwcache_uid(pwcp,pwp,pwbuf,pwlen,uid) ;
	            len = rs ;
	        } /* end if (get by UID) */
	        rs1 = ucygetpw_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ucygetpw-cap) */
	} /* end if (ucygetpw_init) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucygetpw_uid) */

int ucygetpw_stats(UCYGETPW_STATS *usp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (usp) {
	memset(usp,0,sizeof(UCYGETPW_STATS)) ;
	if ((rs = ucygetpw_init()) >= 0) {
	    ucygetpw	*uip = &ucygetpw_data ;
	    if ((rs = ucygetpw_capbegin(uip,-1)) >= 0) {
	        if (uip->pwc == NULL) rs = ucygetpw_begin(uip) ;
	        if (rs >= 0) {
	            ucpwcache		*pwcp = (ucpwcache *) uip->pwc ;
	            ucpwcache_STATS	s ;
	            if ((rs = upwcache_stats(pwcp,&s)) >= 0) {
	                usp->max = uip->max ;
	                usp->ttl = uip->ttl ;
	                usp->nent = s.nentries ;
	                usp->acc = s.total ;
	                usp->phit = s.phits ;
	                usp->nhit = s.nhits ;
	                usp->pmis = s.pmisses ;
	                usp->nmis = s.nmisses ;
	                n = s.nentries ;
	            } /* end if (upwcache_stats) */
	        } /* end if */
	        rs1 = ucygetpw_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ucygetpw-cap) */
	} /* end if (ucygetpw_init) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ucygetpw_stats) */


/* local subroutines (or "private"?) */

static int ucygetpw_capbegin(ucygetpw *uip,int to) noex {
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
/* end subroutine (ucygetpw_capbegin) */

static int ucygetpw_capend(ucygetpw *uip) noex {
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
/* end subroutine (ucygetpw_capend) */

static int ucygetpw_begin(ucygetpw *uip) noex {
	int		rs = SR_OK ;
	if (uip->pwc == NULL) {
	    const int	size = sizeof(ucpwcache) ;
	    void	*p ;
	    if ((rs = uc_libmalloc(size,&p)) >= 0) {
	        const int	max = UCYGETPW_MAX ;
	        const int	ttl = UCYGETPW_TTL ;
	        ucpwcache	*pwcp = (ucpwcache *) p ;
	        if ((rs = upwcache_start(pwcp,max,ttl)) >= 0) {
	            uip->pwc = pwcp ;
	            uip->max = max ;
	            uip->ttl = ttl ;
		}
	        if (rs < 0)
	            uc_libfree(p) ;
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (ucygetpw_begin) */

static int ucygetpw_end(ucygetpw *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->pwc != NULL) {
	    ucpwcache	*pwcp = (ucpwcache *) uip->pwc ;
	    rs1 = upwcache_finish(pwcp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_libfree(uip->pwc) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->pwc = NULL ;
	}
	return rs ;
}
/* end subroutine (ucygetpw_end) */

static void ucygetpw_atforkbefore() noex {
	ucygetpw		*uip = &ucygetpw_data ;
	ucygetpw_capbegin(uip,-1) ;
}
/* end subroutine (ucygetpw_atforkbefore) */

static void ucygetpw_atforkafter() noex {
	ucygetpw		*uip = &ucygetpw_data ;
	ucygetpw_capend(uip) ;
}
/* end subroutine (ucygetpw_atforkafter) */


