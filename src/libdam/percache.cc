/* percache SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* persistent cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

	= 2025-01-18, David A­D­ Morano
	I added the subroutine |percache_getnodename()|.  Why?  I
	am not sure.  There was no existing requirement for this
	addition.  The value does not change very much, if at all
	(during a single boot-up of the system), so it seemed like
	a resonable thing to cache.  I was already caching the NIS
	domain name (which also does not change during a single
	system boot-up) so it seemed reasonable to add support for
	caching the 'nodename'.  I also reviewed the code for
	conformance w/ C++20 (although I am actually compiling
	now-a-days for C++26).

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	percache

	Synopsis:
	int percache_xxx(percache *op,...) noex

	Descrption:
	This was originally written to store permanent (or
	semi-permanent) data items for use in KornShell (KSH) built-in
	commands.  In the original use case (KornShell built-ins)
	there was no need for thread-safety because all built-ins
	are run serially.

	Arguments:
	op		pointer to particular data instance (many are possible)

	Returns:
	>=0		OK, w/ possible useful value
	<0		error (system-return)

	Note:
	We need to (should) use the special memory allocation
	subroutines that are set aside for library use.  These
	memory-allocation-free subroutines are:

	+ lm_mall(3uc)
	+ lm_free(3uc)
	+ libmalloc{xx}

	These subroutines are need when we deal with data that is
	persistent across full instances of command executions.
	That is: these subroutine are need when our data stays valid
	throughout the start and complete exit of a command program.
	Therefore normal conventions about what data was allocated
	and left un-freed do not apply to our dynamic memory usage(s).
	Additionally, ant dynamic data that is allocated and freed
	in the span of time before we exit back to our callers is
	allowed to use any allocation mechanism they want.  Why?
	Because the allocated data is returned (free) before anyone
	else will actually see it.  That is the key.  We only need
	to guard against having allocated memory stay around when
	we return to our callers before we deallocate our allocated
	data.  To someone looking at this code to add or modify it,
	it means that these memory allocations are not tracked like
	normal ones are.  Therefore try not to mess up by creating
	(magining) memory leaks, where there are *none* right now!
	Get it?

	Notes:

	Just so an observer (like myself later on) will not go too
	crazy trying to understand what is going on with the 'struct
	percache' local data, it is a persistent data structure.
	This program can operate as both a regular program (is
	flushed from system memory when it exits) or it can be
	operated as sort of a terminate-stay-resident (TSR) program
	(its data is not flushed when the command exists).  We detect
	which it is (which mode we are executing in) dynamically.
	We do this by simply looking at the persistent data and
	seeing if elements of it are non-zero.  Any non-zero data
	indicates that we have already been executed in the past.
	This data is allocated in the BSS section of our process
	memory map so it is initialized to all-zero on program-load
	(a UNIX standard now for? over twenty years!).

	Hopefully, everything else now makes sense upon inspection
	with this understanding.

	Why do this? Because it speeds things up.  Everything in
	this program is already quite fast, but we have the chance
	of reducing some file-access work with the introduction of
	a persistent data cache.  It is hard to get faster than a
	single file-access (like a shared-memory cache), so anything
	worth doing has to be a good bit faster than that.  Hence,
	pretty much only TSR behavior can beat a single file access.

	Parallelism? There is not any; so we do not have to worry
	about using mutexes or semaphores.  Maybe someday we will
	have to think about parallelism, but probably not any time
	soon!

	OK, now for some of the unpleasantness.  We have to release
	persistent data that was dynamically allocated with the
	PERCACHE facility.  We do this by calling |percache_fini()|
	on our particular data handle.  But this should be done at
	module unload time, so we need to register a subroutine to
	do this that is called at module-unload time.  That subroutine
	will be |ourfini()|.  The registration is only done when we
	are *exiting* this command.  This is done so that we do not
	perform the registration if the PERCACHE facility was never
	used.  The cache facility keeps track within itself whether
	it was used or not (among all possible simulraneously users
	also).  We ask its opinion on that and act accordingly.  Only
	one "user" (command) within the same module will be given
	notice to perform a registration.  However if multiple users
	(commands) do register a |fini()| (needlessly) it is dealt
	with without problems (extras are ignored as expected).
	Welcome to TSR management when in a very dynamic execution
	environment!

	= choice of what data to cache

	We cache data that tends to be difficult (time consuming)
	to get in the first place.  Data which are relatively easy
	to get are not generally handled with this module.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<utmpacc.h>
#include	<getxname.h>		/* |getnodename(3uc)| */
#include	<getx.h>		/* |getnodedomain(3uc)| */
#include	<libmallocxx.h>
#include	<localget.h>
#include	<strwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"percache.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct geter {
	percache	*pcp ;
	cchar		*pr ;
	cchar		**rpp ;
	time_t		dt ;
	geter(percache *o,time_t t,cc *p,cc **r) noex {
	    pcp = o ;
	    dt = t ;
	    pr = p ;
	    rpp = r ;
	} ;
	int operator () (int) noex ;
	int getval(int) noex ;
	int getstr(int) noex ;
	int getx(int,char *,int) noex ;
	int getv(int) noex ;
    } ; /* end struct (geter) */
} /* end namespace */


/* forward references */

static bool	isNoProcs(int) noex ;


/* local variables */

constexpr cint	timeouts[] = {
	(1*60*60),	/* hostid */
	5,		/* nprocs */
	(5*3600),	/* btime (boot-time) */
	20,		/* run-level */
	5, 		/* nusers */
	(1*60*60),	/* nodename */
	(1*60*60),	/* netdomain */
	(1*60*60),	/* nisdomain */
	(5*60),		/* netload */
	(5*60),		/* systat */
	0
} ; /* end array (timeouts) */


/* exported variables */


/* exported subroutines */

int percache_init(percache *pcp) noex {
	int		rs = SR_FAULT ;
	if (pcp) {
	    rs = SR_NOTOPEN ;
	    if (! pcp->f_void) {
		rs = SR_OK ;
		if (! pcp->f_init) {
	            pcp->f_init = true ;
	            pcp->f_initdone = true ;
	        } /* end if (needed initialization) */
	    } /* end if (not voided) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_init) */

int percache_fini(percache *pcp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pcp) {
	    rs = SR_NXIO ;
	    if (! pcp->f_void) {
		pcp->f_void = true ;
	        rs = SR_OK ;
	        if (pcp->f_initdone) {
		    for (int i = 0 ; i < pertype_overlast ; i += 1) {
		        char	*dp = pcp->items[i].data ;
	                if (dp) {
		            rs1 = lm_free(dp) ;
		            if (rs >= 0) rs = rs1 ;
		            pcp->items[i].data = nullptr ;
	                }
			pcp->items[i].t = 0 ;
			pcp->items[i].v = 0 ;
		    } /* end for */
		    pcp->f_init = false ;
		    pcp->f_initdone = false ;
	        } /* end if (was inited) */
	    } /* end if (not-voided) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_fini) */

/* is a "fini" registration required? false=no, true=yes */
int percache_finireg(percache *pcp) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (pcp) {
	    rs = SR_OK ;
	    if (pcp->f_init) {
		if ((f = (! pcp->f_finireg)) > 0) {
	            pcp->f_finireg = true ;
		}
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (percache_finireg) */

int percache_invalidate(percache *pcp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pcp) {
	    rs = SR_OK ;
	    if (pcp->f_init) {
		for (int i = 0 ; i < pertype_overlast ; i += 1) {
		    char	*dp = pcp->items[i].data ;
	            if (dp) {
		        rs1 = lm_free(dp) ;
		        if (rs >= 0) rs = rs1 ;
		        pcp->items[i].data = nullptr ;
	            }
		    pcp->items[i].t = 0 ;
		    pcp->items[i].v = 0 ;
		} /* end for */
	    } /* end if (initialized) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_invalidate) */

int percache_gethostid(percache *pcp,time_t dt,uint *hip) noex {
	cint		pt = pertype_hostid ;
	int		rs = SR_FAULT ;
	if (pcp && hip) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        if ((dt - vt) > to) {
	            long	lv = gethostid() ;
	            pcp->items[pt].t = uint(dt) ;
	            pcp->items[pt].v = uint(lv) ;
	        } /* end if */
	        *hip = pcp->items[pt].v ;
	    } /* end if (init) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_gethostid) */

int percache_getbtime(percache *pcp,time_t dt,time_t *btp) noex {
	cint		pt = pertype_btime ;
	int		rs = SR_FAULT ;
	if (pcp && btp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        if ((dt - vt) > to) {
	            if (time_t bt ; (rs = utmpacc_boottime(&bt)) >= 0) {
	                pcp->items[pt].t = uint(dt) ;
	                pcp->items[pt].v = uint(bt) ;
	            }
	        } /* end if (update needed) */
	        *btp = pcp->items[pt].v ;
	    } /* end if (init) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_getbtime) */

int percache_getnprocs(percache *pcp,time_t dt) noex {
	cint		pt = pertype_nprocs ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (pcp) {
    	    geter go(pcp,dt,nullptr,nullptr) ;
	    rs = go(pt) ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (percache_getnprocs) */

int percache_getrunlevel(percache *pcp,time_t dt) noex {
	cint		pt = pertype_runlevel ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (pcp) {
    	    geter go(pcp,dt,nullptr,nullptr) ;
	    rs = go(pt) ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (percache_getrunlevel) */

int percache_getnusers(percache *pcp,time_t dt) noex {
	cint		pt = pertype_nusers ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (pcp) {
    	    geter go(pcp,dt,nullptr,nullptr) ;
	    rs = go(pt) ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (percache_getnusers) */

int percache_getnodename(percache *pcp,time_t dt,cchar **rpp) noex {
    	int		rs = SR_FAULT ;
	if (pcp) {
    	    geter go(pcp,dt,nullptr,rpp) ;
	    rs = go(pertype_nodename) ;
	}
	return rs ;
}

int percache_getnetdomain(percache *pcp,time_t dt,cchar **rpp) noex {
    	int		rs = SR_FAULT ;
	if (pcp) {
    	    geter go(pcp,dt,nullptr,rpp) ;
	    rs = go(pertype_netdomain) ;
	}
	return rs ;
}
/* end subroutine (percache_getnetdomain) */

int percache_getnisdomain(percache *pcp,time_t dt,cchar **rpp) noex {
    	int		rs = SR_FAULT ;
	if (pcp) {
    	    geter go(pcp,dt,nullptr,rpp) ;
	    rs = go(pertype_nisdomain) ;
	}
	return rs ;
}
/* end subroutine (percache_getnisdomain) */

int percache_getnetload(percache *pcp,time_t dt,cchar *pr,cchar **rpp) noex {
    	int		rs = SR_FAULT ;
	if (pcp && pr) {
    	    geter go(pcp,dt,pr,rpp) ;
	    rs = go(pertype_netload) ;
	}
	return rs ;
}
/* end subroutine (percache_getnetload) */

int percache_getsystat(percache *pcp,time_t dt,cchar *pr,cchar **rpp) noex {
    	int		rs = SR_FAULT ;
	if (pcp && pr) {
    	    geter go(pcp,dt,pr,rpp) ;
	    rs = go(pertype_systat) ;
	}
	return rs ;
}
/* end subroutine (percache_getsystat) */


/* local subroutines */

int geter::operator () (int pt) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = percache_init(pcp)) >= 0) {
	    if (dt == 0) dt = getustime ;
	    switch (pt) {
	    case pertype_nprocs:
	    case pertype_runlevel:
	    case pertype_nusers:
		rs = getval(pt) ;
	        break ;
	    case pertype_nodename:
	    case pertype_netdomain:
	    case pertype_nisdomain:
	    case pertype_netload:
	    case pertype_systat:
		rs = getstr(pt) ;
	        break ;
	    } /* end switch */
	} /* end if (init) */
	return (rs >= 0) ? len : rs ;
}
/* end method (geter::operator) */

int geter::getval(int pt) noex {
    	int		rs = SR_OK ;
	int		n = 0 ;
	custime		vt = pcp->items[pt].t ;
	cint		to = timeouts[pt] ;
	n = pcp->items[pt].v ;
	if ((dt - vt) > to) {
	    if ((rs = getv(pt)) >= 0) {
	        n = rs ;
	        pcp->items[pt].t = uint(dt) ;
	        pcp->items[pt].v = n ;
	    }
	} /* end if (update needed) */
	return (rs >= 0) ? n : rs ;
}
/* end method (geter::gerval) */

int geter::getstr(int pt) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	custime	vt = pcp->items[pt].t ;
	cint	to = timeouts[pt] ;
	char	*dp = pcp->items[pt].data ;
	len = pcp->items[pt].v ;
	if (((dt - vt) > to) || (dp == nullptr)) {
	    if (char *dbuf ; (rs = libmalloc_nn(&dbuf)) >= 0) {
	        cint	dlen = rs ;
		if (dp) {
		    lm_free(dp) ;
		    pcp->items[pt].data = nullptr ;
		}
	        if ((rs = getx(pt,dbuf,dlen)) >= 0) {
		    len = rs ;
		    if (char *bp ; (rs = lm_mall((len+1),&bp)) >= 0) {
			strwcpy(bp,dbuf,len) ;
		        pcp->items[pt].data = bp ;
	                pcp->items[pt].t = uint(dt) ;
		        pcp->items[pt].v = len ;
	            } /* end if (memory-allocation) */
		} /* end if (getx) */
		rs1 = lm_free(dbuf) ;
		if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	} /* end if (update needed) */
	if (rpp) {
	    *rpp = (rs >= 0) ? pcp->items[pt].data : nullptr ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (geter::getstr) */

int geter::getv(int pt) noex {
    	int		rs = SR_BUGCHECK ;
	switch (pt) {
	case pertype_nprocs:
	    if ((rs = uc_nprocs(0)) < 0) {
		if (isNoProcs(rs)) {
		    rs = SR_OK ;
		}
	    }
	    break ;
	case pertype_runlevel:
	    rs = utmpacc_runlevel() ;
	    break ;
	case pertype_nusers:
	    rs = utmpacc_users(0) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (geter::getv) */

/* this (below) kind-of serves as our "virtual" function w/o the b*llsh*t */
int geter::getx(int pt,char *dbuf,int dlen) noex {
    	int		rs = SR_BUGCHECK ;
	switch (pt) {
	case pertype_nodename:
	    rs = getnodename(dbuf,dlen) ;
	    break ;
	case pertype_netdomain:
	    rs = getinetdomain(dbuf,dlen) ;
	    break ;
	case pertype_nisdomain:
	    rs = getnisdomain(dbuf,dlen) ;
	    break ;
	case pertype_netload:
	    rs = localgetnetload(pr,dbuf,dlen) ;
	    break ;
	case pertype_systat:
	    rs = localgetsystat(pr,dbuf,dlen) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (geter::getx) */

static bool isNoProcs(int rs) noex {
	bool		f = false ;
	f = f || isNotPresent(rs) ;
	f = f || (rs == SR_NOSYS) ;
	return f ;
}
/* end subroutine (isNoProcs) */


