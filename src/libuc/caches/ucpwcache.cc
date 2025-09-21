/* ucpwcache SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* get UNIX® password entries (w/ cache) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	ucpwcache

	Description:
	This module serves as a per-process cache for UNIX® password
	entries.  Since we are basically dealing with global data,
	we need to make the establishment of it multi-thread safe.
	We also want fork safety.  Yes, we want everything, including
	cleanup on module unloading (since, yes, we could all be
	inside a loadable and unloadble module!).  For these purposes
	we employ the basic (and not so basic) means of accomplishing
	this.  See the code for our various machinations.

	Notes:

	Q. The old age question: do these (public) subroutines need
	to be multi-thread-safe?
	A. What do you think?

	Q. Why cannot we just use a POSIX® mutex-lock around the
	guts of the public subroutines?
	A. Because those "guts" might do some complex 
	operating-system-like things that would lead to a deadlock
	(because perhaps there is a |uc_fork(3uc)| in there, for
	example)!  It is *much* better to be safe than sorry.

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
#include	<cstddef>		/* |nullptr_t(3c++)| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<atomic>		/* |atomic_int(3c++)| */
#include	<usystem.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<ptc.h>
#include	<pwcache.h>
#include	<localmisc.h>

#include	"ucpwcache.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::atomic_int ;			/* type */
using std::nothrow ;			/* constant */


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
	ucpwcacheco_overlast
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
	int operator () (int = -1) noex ;
	operator int () noex {
	    return (*this)() ;
	} ;
    } ;
    struct ucpwcache {
	ptm		mx ;		/* data mutex */
	ptc		cv ;		/* condition variable */
	pwcache		*pwc ;		/* PW cache (allocated) */
	ucpwcache_co	init ;
	ucpwcache_co	fini ;
	ucpwcache_co	opcheck ;
	ucpwcache_co	opbegin ;
	ucpwcache_co	opend ;
	ucpwcache_co	capbegin ;
	ucpwcache_co	capend ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		fcapture ;	/* capture flag */
	atomic_int	waiters ;
	int		nmax ;		/* max entries */
	int		ttl ;
	ucpwcache() noex {
	    init(this,ucpwcacheco_init) ;
	    fini(this,ucpwcacheco_fini) ;
	    opcheck(this,ucpwcacheco_opcheck) ;
	    opbegin(this,ucpwcacheco_opbegin) ;
	    opend(this,ucpwcacheco_opend) ;
	    capbegin(this,ucpwcacheco_capbegin) ;
	    capend(this,ucpwcacheco_capend) ;
	} ;
	int	iinit() noex ;
	int	ifini() noex ;
	int	icapbegin(int) noex ;
	int	icapend() noex ;
	int	iopcheck() noex ;
	int	iopbegin() noex ;
	int	iopend() noex ;
	int	name(ucentpw *,char *,int,cchar *) noex ;
	int	uid(ucentpw *,char *,int,uid_t) noex ;
	int	getstat(ucpwcache_st *) noex ;
	destruct ucpwcache() noex {
	    if (cint rs = fini ; rs < 0) {
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

int ucpwcache_name(ucentpw *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	return ucpwcache_data.name(pwp,pwbuf,pwlen,un) ;
}

int ucpwcache_uid(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	return ucpwcache_data.uid(pwp,pwbuf,pwlen,uid) ;
}

int ucpwcache_getstat(ucpwcache_st *usp) noex {
	return ucpwcache_data.getstat(usp) ;
}


/* local subroutines */

int ucpwcache::iinit() noex {
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
	            int		rsl = SR_OK ; /* XX GCC complaint */
	            if (!finit) {
		        rsl = SR_LOCKLOST ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            }
	            return rsl ;
	        } ; /* end lambda */
	        rs = tw(lamb) ;
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (ucpwcache::iinit) */

int ucpwcache::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    if (pwc) {
	        rs1 = opend() ;
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
/* end method (ucpwcache::ifini) */

int ucpwcache::name(ucentpw *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pwp && pwbuf && un) {
	    memclear(pwp) ;		/* <- noted */
	    if ((rs = init) >= 0) {
	        if ((rs = capbegin) >= 0) {
	            if ((rs = opcheck) >= 0) {
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

int ucpwcache::uid(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pwp && pwbuf) {
	    memclear(pwp) ;		/* <- noted */
	    if ((rs = init()) >= 0) {
	        if ((rs = capbegin) >= 0) {
	            if ((rs = opcheck) >= 0) {
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

int ucpwcache::getstat(ucpwcache_st *usp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (usp) {
	    memclear(usp) ;		/* dangerous */
	    if ((rs = init()) >= 0) {
	        if ((rs = capbegin) >= 0) {
	            if ((rs = opcheck) >= 0) {
	                pwcache_st s ; 
			if ((rs = pwcache_getstat(pwc,&s)) >= 0) {
	                    usp->nmax = nmax ;
	                    usp->ttl = ttl ;
	                    usp->nent = s.nentries ;
	                    usp->acc = s.total ;
	                    usp->phit = s.phits ;
	                    usp->nhit = s.nhits ;
	                    usp->pmis = s.pmisses ;
	                    usp->nmis = s.nmisses ;
	                    n = s.nentries ;
	                } /* end if (ucpwcache_getstat) */
	            } /* end if */
	            rs1 = capend() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ucpwcache-cap) */
	    } /* end if (ucpwcache_init) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end method (ucpwcache::getstat) */

int ucpwcache::icapbegin(int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mx.lockbegin(to)) >= 0) {
	    waiters += 1 ;
	    while ((rs >= 0) && fcapture) { /* busy */
	        rs = cv.wait(&mx,to) ;
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
/* end method (ucpwcache::icapbegin) */

int ucpwcache::icapend() noex {
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
/* end method (ucpwcache::icapend) */

int ucpwcache::iopcheck() noex {
	int		rs = SR_OK ;
	if (!pwc) {
	    rs = iopbegin() ;
	}
	return rs ;
}
/* end method (ucpwcache::iopcheck) */

int ucpwcache::iopbegin() noex {
	int		rs = SR_OK ;
	if (pwc == nullptr) {
	    cint	esz = szof(ucpwcache) ;
	    if (void *vp ; (rs = lm_mall(esz,&vp)) >= 0) {
	        cint		amax = UCPWCACHE_MAX ;
	        cint		attl = UCPWCACHE_TTL ;
	        pwcache		*pwcp = (pwcache *) vp ;
	        if ((rs = pwcache_start(pwcp,amax,attl)) >= 0) {
	            pwc = pwcp ;
	            nmax = amax ;
	            ttl = attl ;
		}
	        if (rs < 0) {
	            lm_free(vp) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (needed initialization) */
	return rs ;
}
/* end method (ucpwcache::iopbegin) */

int ucpwcache::iopend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pwc != nullptr) {
	    pwcache	*pwcp = (pwcache *) pwc ;
	    {
	        rs1 = pwcache_finish(pwcp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = lm_free(pwc) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    pwc = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucpwcache::iopend) */

static void ucpwcache_atforkbefore() noex {
	ucpwcache_data.capbegin(-1) ;
}
/* end subroutine (ucpwcache_atforkbefore) */

static void ucpwcache_atforkafter() noex {
	ucpwcache_data.capend() ;
}
/* end subroutine (ucpwcache_atforkafter) */

static void ucpwcache_exit() noex {
	if (cint rs = ucpwcache_data.fini ; rs < 0) {
	    ulogerror("ucpwcache",rs,"exit-fini") ;
	}
}
/* end subroutine (ucpwcache_exit) */

int ucpwcache_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ucpwcacheco_init:
		rs = op->iinit() ;
		break ;
	    case ucpwcacheco_fini:
		rs = op->ifini() ;
		break ;
	    case ucpwcacheco_capbegin:
		rs = op->icapbegin(a) ;
		break ;
	    case ucpwcacheco_capend:
		rs = op->icapend() ;
		break ;
	    case ucpwcacheco_opcheck:
		rs = op->iopcheck() ;
		break ;
	    case ucpwcacheco_opbegin:
		rs = op->iopbegin() ;
		break ;
	    case ucpwcacheco_opend:
		rs = op->iopend() ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ucpwcache::operator) */


