/* percache SUPPORT */
/* lang=C++20 */

/* persistent cache */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Synopsis:
	int percache_xxx(PERCACHE *op,...) noex

	where:
	op		pointer to particular data instance (many are possible)

	Returns:
	>=0		OK, w/ possible useful value
	<0		error (system-return)

	Note:
	We need to (should) use the memory allocation subroutines:

	+ uc_libmalloc(3uc)
	+ uc_libfree(3uc)

	since we deal with data that is persistent across full
	instances of command executions.  That is: our data stays
	valid throughout the start and complete exit of a command
	program.  Therefore normal conventions about what data was
	allocated and left un-freed do not apply to our dynamic
	memory usage(s).  To someone looking at this code to add or
	modify it, it means that these memory allocations are not
	tracked like normal ones are.  Therefore try not to mess up
	by creating memory leaks, where there are *none* right now!
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<utmpacc.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<localget.h>
#include	<strwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"percache.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool	isNoProcs(int rs) noex ;


/* local variables */

static constexpr cint	timeouts[] = {
	(1*60*60),	/* hostid */
	5,		/* nprocs */
	(5*3600),	/* btime (boot-time) */
	20,		/* run-level */
	5, 		/* nusers */
	(1*60*60),	/* sysdomain */
	(5*60),		/* netload */
	(5*60),		/* systat */
	0
} ;


/* exported variables */


/* exported subroutines */

int percache_init(PERCACHE *pcp) noex {
	int		rs = SR_FAULT ;
	if (! pcp->f_init) {
	    rs = SR_OK ;
	    pcp->f_init = true ;
	    pcp->f_initdone = true ;
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (percache_init) */

int percache_fini(PERCACHE *pcp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pcp) {
	    rs = SR_OK ;
	    if (pcp->f_initdone) {
	        if (pcp->sysdomain != nullptr) {
		    rs1 = uc_libfree(pcp->sysdomain) ;
		    if (rs >= 0) rs = rs1 ;
		    pcp->sysdomain = nullptr ;
	        }
	        memclear(pcp) ;
	    } /* end if (was inited) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_fini) */

/* is a "fini" registration required? */
int percache_finireg(PERCACHE *pcp) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (pcp) {
	    rs = SR_OK ;
	    if (pcp->f_init) {
	        f = ((! pcp->f_finireg) && (pcp->sysdomain != nullptr)) ;
	        if (f) pcp->f_finireg = true ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (percache_finireg) */

int percache_invalidate(PERCACHE *pcp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pcp) {
	    rs = SR_OK ;
	    if (pcp->f_init) {
	        cint	sz = int(sizeof(PERCACHE_ITEM) * pertype_overlast) ;
	        memset(pcp->items,0,sz) ;
	        if (pcp->sysdomain != nullptr) {
	            rs1 = uc_libfree(pcp->sysdomain) ;
	            if (rs >= 0) rs = rs1 ;
	            pcp->sysdomain = nullptr ;
	        }
	    } /* end if (initialized) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_invalidate) */

int percache_gethostid(PERCACHE *pcp,time_t dt,uint *hip) noex {
	cint		pt = pertype_hostid ;
	int		rs = SR_FAULT ;
	if (pcp && hip) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        if ((dt - vt) > to) {
	            uint	uv = gethostid() ;
	            pcp->items[pt].t = dt ;
	            pcp->items[pt].v = uv ;
	        } /* end if */
	        *hip = pcp->items[pt].v ;
	    } /* end if (init) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_gethostid) */

int percache_getnprocs(PERCACHE *pcp,time_t dt) noex {
	cint		pt = pertype_nprocs ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (pcp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        n = pcp->items[pt].v ;
	        if ((dt - vt) > to) {
	            if ((rs = uc_nprocs(0)) >= 0) {
	                n = rs ;
	            } else if (isNoProcs(rs)) {
		        rs = SR_OK ;
		    }
	            pcp->items[pt].t = dt ;
	            pcp->items[pt].v = n ;
	        } /* end if (update needed) */
	    } /* end if (init) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (percache_getnprocs) */

int percache_getbtime(PERCACHE *pcp,time_t dt,time_t *btp) noex {
	cint		pt = pertype_btime ;
	int		rs = SR_FAULT ;
	if (pcp && btp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        if ((dt - vt) > to) {
	            time_t	bt ;
	            if ((rs = utmpacc_boottime(&bt)) >= 0) {
	                pcp->items[pt].t = dt ;
	                pcp->items[pt].v = bt ;
	            }
	        } /* end if (update needed) */
	        *btp = pcp->items[pt].v ;
	    } /* end if (init) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (percache_getbtime) */

int percache_getrunlevel(PERCACHE *pcp,time_t dt) noex {
	cint		pt = pertype_runlevel ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (pcp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime	vt = pcp->items[pt].t ;
	        cint	to = timeouts[pt] ;
	        n = pcp->items[pt].v ;
	        if ((dt - vt) > to) {
	            if ((rs = utmpacc_runlevel()) >= 0) {
	                n = rs ;
	                pcp->items[pt].t = dt ;
	                pcp->items[pt].v = n ;
	            }
	        } /* end if (update needed) */
	    } /* end if (init) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (percache_getrunlevel) */

int percache_getnusers(PERCACHE *pcp,time_t dt) noex {
	cint		pt = pertype_nusers ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (pcp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        n = pcp->items[pt].v ;
	        if ((dt - vt) > to) {
	            if ((rs = utmpacc_users(0)) >= 0) {
	                n = rs ;
	                pcp->items[pt].t = dt ;
	                pcp->items[pt].v = n ;
	            }
	        } /* end if (update needed) */
	    } /* end if (init) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (percache_getnusers) */

int percache_getsysdomain(PERCACHE *pcp,time_t dt,cchar **rpp) noex {
	cint		pt = pertype_sysdomain ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pcp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        len = pcp->items[pt].v ;
	        if (((dt - vt) > to) || (pcp->sysdomain == nullptr)) {
	            char	*dbuf{} ;
		    if ((rs = malloc_hn(&dbuf)) >= 0) {
	                cint	dlen = rs ;
		        if (pcp->sysdomain != nullptr) {
		            uc_libfree(pcp->sysdomain) ;
		            pcp->sysdomain = nullptr ;
		        }
	                if ((rs = getsysdomain(dbuf,dlen)) >= 0) {
		            char	*bp ;
		            len = rs ;
		            if ((rs = uc_libmalloc((len+1),&bp)) >= 0) {
			        strwcpy(bp,dbuf,len) ;
		                pcp->sysdomain = bp ;
	                        pcp->items[pt].t = dt ;
	    	                pcp->items[pt].v = len ;
	                    } /* end if (memory-allocation) */
		        } /* end if (getsysdomain) */
		        rs1 = uc_free(dbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (update needed) */
	        if (rpp) {
		    *rpp = (rs >= 0) ? pcp->sysdomain : nullptr ;
	        }
	    } /* end if (init) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (percache_getsysdomain) */

int percache_netload(PERCACHE *pcp,time_t dt,cchar *pr,cchar **rpp) noex {
	cint		pt = pertype_netload ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pcp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        len = pcp->items[pt].v ;
	        if (((dt - vt) > to) || (pcp->netload == nullptr)) {
	            char	*dbuf{} ;
		    if ((rs = malloc_hn(&dbuf)) >= 0) {
	                cint	dlen = rs ;
		        if (pcp->netload != nullptr) {
		            uc_libfree(pcp->netload) ;
		            pcp->netload = nullptr ;
		        }
	                if ((rs = localgetnetload(pr,dbuf,dlen)) >= 0) {
		            char	*bp ;
		            len = rs ;
		            if ((rs = uc_libmalloc((len+1),&bp)) >= 0) {
			        strwcpy(bp,dbuf,len) ;
		                pcp->netload = bp ;
	                        pcp->items[pt].t = dt ;
	    	                pcp->items[pt].v = len ;
	                    } /* end if (memory-allocation) */
		        } /* end if (localgetnetload) */
		        rs1 = uc_free(dbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (update needed) */
	        if (rpp) {
		    *rpp = (rs >= 0) ? pcp->netload : nullptr ;
	        }
	    } /* end if (init) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (percache_netload) */

int percache_systat(PERCACHE *pcp,time_t dt,cchar *pr,cchar **rpp) noex {
	cint		pt = pertype_systat ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pcp) {
	    if ((rs = percache_init(pcp)) >= 0) {
	        custime		vt = pcp->items[pt].t ;
	        cint		to = timeouts[pt] ;
	        len = pcp->items[pt].v ;
	        if (((dt - vt) > to) || (pcp->systat == nullptr)) {
	            char	*dbuf{} ;
		    if ((rs = malloc_hn(&dbuf)) >= 0) {
		        cint	dlen = rs ;
		        if (pcp->systat != nullptr) {
		            uc_libfree(pcp->systat) ;
		            pcp->systat = nullptr ;
		        }
	                if ((rs = localgetsystat(pr,dbuf,dlen)) >= 0) {
		            char	*bp ;
			    len = rs ;
		            if ((rs = uc_libmalloc((len+1),&bp)) >= 0) {
			        strwcpy(bp,dbuf,len) ;
		                pcp->systat = bp ;
	                        pcp->items[pt].t = dt ;
	    	                pcp->items[pt].v = len ;
	                    } /* end if (memory-allocation) */
		        } /* end if (localgetsystat) */
		        rs1 = uc_free(dbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (update needed) */
	        if (rpp) {
		    *rpp = (rs >= 0) ? pcp->systat : nullptr ;
	        }
	    } /* end if (init) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (percache_systat) */

static bool isNoProcs(int rs) noex {
	bool		f = false ;
	f = f || isNotPresent(rs) ;
	f = f || (rs == SR_NOSYS) ;
	return f ;
}
/* end subroutine (isNoProcs) */


