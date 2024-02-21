/* ucpwcache SUPPORT */
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
#include	<csignal>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>		/* |memclear(3u)| */
#include	<timewatch.hh>
#include	<ptm.h>
#include	<ptc.h>
#include	<pwcache.h>
#include	<localmisc.h>

#include	"ucpwcache.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum ucpwcachecos {
	ucpwcacheco_init,
	ucpwcacheco_fini,
	ucpwcacheco_capbegin,
	ucpwcacheco_capend,
	ucpwcacheco_opcheck,
	ucpwcacheco_opbegin,
	ucpwcacheco_opend,
	ucpwcacheco_overlast,
} ;

namespace {
    struct ucpwcache ;
    struct ucpwcache_co {
	ucpwcache	*op = nullptr ;
	int		w = -1 ;
	void operator () (ucpwcache *p,int aw) noex {
	    op = p ;
	    w = aw ;
	}
	operator int () noex ;
	operator () (int = -1) noex ;
    } ;
    struct ucpwcache {
	ptm		mx ;		/* data mutex */
	ptc		cv ;		/* condition variable */
	pwcache		*pwc ;		/* PW cache (allocated) */
	pcpwcache_co	check ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		fcapture ;	/* capture flag */
	aflag		waiters ;
	int		nmax ;		/* max entries */
	int		ttl ;
	ucpwcache() noex {
	    opcheck(this,ucpwcacheco_check) ;
	} ;
	int	init() noex ;
	int	fini() noex ;
	int	capbegin(int) noex ;
	int	capend() noex ;
	int	begin() noex ;
	int	end() noex ;
	int	capbegin(int) noex ;
	int	capend() noex ;
	int	name(PASSWD *,char *,int,cchar *) noex ;
	int	uid(PASSWD *,char *,int,uid_t) noex ;
	int	stats(ucpwcache_st *) noex ;
	~ucpwcache() noex {
	    cint	rs = fini() ;
	    if (rs < 0) {
		ulogerror("ucpwcache",rs,"dtor-fini") ;
	    }
	} ; /* end dtor */
    } ; /* end struct (ucpwcache) */
}


/* forward references */

extern "C" {
    static void	ucpwcache_atforkbefore() noex ;
    static void	ucpwcache_atforkafter() noex ;
    static void	ucpwcache_exit() noex ;
}


/* local variables */

static ucpwcache	ucpwcache_data ;


/* exported variables */


/* exported subroutines */

int ucpwcache_init() noex {
	return ucpwcache_data.init() ;
}

int ucpwcache_fini() noex {
	return ucpwcache_data.fini() ;
}

int ucpwcache_name(PASSWD *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	return ucpwcache_data.name(pwp,pwbuf,pwlen,un) ;
}

int ucpwcache_uid(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	return ucpwcache_data.uid(pwp,pwbuf,pwlen,uid) ;
}

int ucpwcache_stats(ucpwcache_st *usp) noex {
	return ucpwcache_data.stats(usp) ;
}


/* local subroutines */

int ucpwcache::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {	/* <- the money shot */
	        if ((rs = mx.create) >= 0) {
	            if ((rs = cv.create) >= 0) {
	    	        void_f	b = ucpwcache_atforkbefore ;
	    	        void_f	a = ucpwcache_atforkafter ;
	                if ((rs = uc_atfork(b,a,a)) >= 0) {
	                    if ((rs = uc_atexit(ucpwcache_exit)) >= 0) {
	                        finitdone = true ;
	                        f = true ;
	                    }
	                    if (rs < 0) {
	                        uc_atforkexpunge(b,a,a) ;
			    }
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    cv.destroy() ;
		        }
	            } /* end if (ptc_create) */
	            if (rs < 0) {
	                mx.destroy() ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
	        }
	    } else if (!finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;
	            } else if (finitdone) {
		        rs = 1 ;
	            }
	            return rs ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (ucpwcache::init) */

int ucpwcache::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    if (pwc) {
	        rs1 = end() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = ucpwcache_atforkbefore ;
	        void_f	a = ucpwcache_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = cv.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finitdone = false ;
	    finit = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end method (ucpwcache::fini) */

int ucpwcache::name(PASSWD *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pwp && pwbuf && un) {
	    memclear(pwp) ;
	    if ((rs = init()) >= 0) {
	        if ((rs = capbegin(-1)) >= 0) {
	            if (pwc == nullptr) rs = begin() ;
	            if (rs >= 0) {
	                pwcache		*pwcp = (pwcache *) pwc ;
	                rs = pwcache_lookup(pwcp,pwp,pwbuf,pwlen,un) ;
	                len = rs ;
	            } /* end if (lookup) */
	            rs1 = capend() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ucpwcache-cap) */
	    } /* end if (ucpwcache_init) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucpwcache_name) */

int ucpwcache::uid(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pwp && pwbuf) {
	    memclear(pwp) ;
	    if ((rs = init()) >= 0) {
	        if ((rs = capbegin(-1)) >= 0) {
	            if (pwc == nullptr) rs = begin() ;
	            if (rs >= 0) {
	                pwcache		*pwcp = (pwcache *) pwc ;
	                rs = pwcache_uid(pwcp,pwp,pwbuf,pwlen,uid) ;
	                len = rs ;
	            } /* end if (get by UID) */
	            rs1 = capend() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ucpwcache-cap) */
	    } /* end if (ucpwcache::init) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ucpwcache_uid) */

int ucpwcache::stats(ucpwcache_st *usp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (usp) {
	    memclear(usp) ;		/* dangerous */
	    if ((rs = init()) >= 0) {
	        if ((rs = capbegin(-1)) >= 0) {
	            if (pwc == nullptr) rs = begin() ;
	            if (rs >= 0) {
	                pwcache		*pwcp = (ucpwcache *) pwc ;
	                ucpwcache_st	s{} ;
	                if ((rs = pwcache_stats(pwcp,&s)) >= 0) {
	                    usp->nmax = nmax ;
	                    usp->ttl = ttl ;
	                    usp->nent = s.nentries ;
	                    usp->acc = s.total ;
	                    usp->phit = s.phits ;
	                    usp->nhit = s.nhits ;
	                    usp->pmis = s.pmisses ;
	                    usp->nmis = s.nmisses ;
	                    n = s.nentries ;
	                } /* end if (ucpwcache_stats) */
	            } /* end if */
	            rs1 = capend() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ucpwcache-cap) */
	    } /* end if (ucpwcache_init) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ucpwcache_stats) */

int ucpwcache::capbegin(int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin(to)) >= 0) {
	    waiters += 1 ;
	    while ((rs >= 0) && fcapture) { /* busy */
	        rs = cv.waiter(&mx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        fcapture = true ;
	    }
	    waiters -= 1 ;
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end method (ucpwcache::capbegin) */

int ucpwcache::capend() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin) >= 0) {
	    fcapture = false ;
	    if (waiters > 0) {
	        rs = cv.signal ;
	    }
	    rs1 = mx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end method (ucpwcache::capend) */

int ucpwcache::begin() noex {
	int		rs = SR_OK ;
	if (pwc == nullptr) {
	    cint	esz = sizeof(ucpwcache) ;
	    void	*vp{} ;
	    if ((rs = uc_libmalloc(esz,&vp)) >= 0) {
	        cint		nmax = UCPWCACHE_MAX ;
	        cint		ttl = UCPWCACHE_TTL ;
	        ucpwcache	*pwcp = (ucpwcache *) vp ;
	        if ((rs = ucpwcache_start(pwcp,nmax,ttl)) >= 0) {
	            pwc = pwcp ;
	            nmax = nmax ;
	            ttl = ttl ;
		}
	        if (rs < 0) {
	            uc_libfree(vp) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end method (ucpwcache::begin) */

int ucpwcache::end() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pwc != nullptr) {
	    pwcache	*pwcp = (pwcache *) pwc ;
	    {
	        rs1 = pwcache_finish(pwcp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uc_libfree(pwc) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    pwc = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucpwcache_end) */

static void ucpwcache_atforkbefore() noex {
	ucpwcache_data.capbegin(-1) ;
}
/* end subroutine (ucpwcache_atforkbefore) */

static void ucpwcache_atforkafter() noex {
	ucpwcache_data.capend() ;
}
/* end subroutine (ucpwcache_atforkafter) */

static void ucpwcache_exit() noex {
	cint		rs = ucpwcache_data.fini() ;
	if (rs < 0) {
	    ulogerror("ucpwcache",rs,"exit-fini") ;
	}
}
/* end subroutine (ucpwcache_exit) */


