/* ucmemalloc SUPPORT (3uc) */
/* lang=C++20 */

/* memory allocation facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This subroutine was originally written.

	= 2001-02-01, David A­D­ Morano
	I expanded the storage table size somewhat for larger
	programs.  We are handling larger amounts of data now-a-days!

	= 2011-04-12, David A­D­ Morano
	I made a few changes for the new decade.  Ya, time flies when
	one is coding day and night. The changes are:

	1. I removed support for linking in the |libmalloc| debugging
	library (was supplied by some vendors).  Find other ways to
	debug if you have to. There was not really much to this in
	the first place, so I do not think it will be largely missed.

	2. I replaced my own beloved memory usage tracking code
	with a new implementation (a new object that I just coded
	up) that uses the the C++11 hash table capability (called
	|unordered_map| in C++11 terminology).  This new organization
	provides a better "separation of concerns" between a) the
	memory allocation process per se and b) the process of
	tracking the allocated memory blocks themselves.  The prior
	behavior was not really like me.  I should have done this
	a long time ago (in keeping with my more normal behavior).

	3. I now return statistics in a new structure named
	|ucmemalloc_stats|.  I coded this sort of thing in other
	places I have worked, but not yet here in my own code
	(below).  I generally do not have access to code that I
	create for others (proprietary copyright and all of that).
	But I tend to follow my own recollection when implementing
	certain features over and over again over some decades.

*/

/* Copyright © 1998,2001,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is the friendly version of the standrd |malloc(3c)|
	subroutine and its various friends.  These subroutines are
	not interpositioned on the standard versions but rather have
	their own symbol names.  The main feature of these subroutines
	is to provide an optional tracking of the allocated memory
	usage.

	API:
	uc_mallset()	turns on memory tracking
	uc_malloc()	analogous to the standard 'malloc(3c)'
	uc_valloc()	analogous to the standard 'valloc(3c)'
	uc_calloc()	analogous to the standard 'calloc(3c)'
	uc_realloc()	analogous to the standard 'realloc(3c)'
	uc_free()	analogous to the standard 'free(3c)'
	uc_mallpresent(cvoid *a) noex
	uc_mallout(ulong *rp) noex
	uc_mallstats(ucmemalloc_stats *statp) noex

	Notes: 
	Because these subroutines are used *everywhere* they
	really have to be MT-safe (ya, for real).  They are not by
	themselves Async-Signal-Safe, but you can do that yourself
	if you want to.  Oh, they are Fork-Safe also.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<sysval.hh>
#include	<memtrack.hh>
#include	<localmisc.h>

#include	"uclibmemalloc.h"
#include	"ucmemalloc.h"
#include	"ucmallreg.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    enum ucmemallocmems {
	ucmemallocmem_init,
	ucmemallocmem_fini,
	ucmemallocmem_overlast
    } ;
    struct ucmemalloc ;
    struct ucmemalloc_co {
	ucmemalloc	*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (ucmemalloc *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ; /* end struct (ucmemalloc_co) */
    struct ucmemalloc {
	typedef ucmemalloc_stats	statblock ;
	ptm		mx ;		/* object mutex */
	memtrack	mt ;
	statblock	st{} ;
	ucmemalloc_co	init ;
	ucmemalloc_co	fini ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		ftrack ;
	int mallcount(ulong *) noex ;
	int mallset(int) noex ;
	int trackstart(int) noex ;
	int trackstarter(int) noex ;
	int trackfinish() noex ;
	int trackmalloc(int,void *) noex ;
	int trackcalloc(int,int,void *) noex ;
	int trackrealloc(cvoid *,int,void *) noex ;
	int trackfree(cvoid *) noex ;
	int trackreg(cvoid *,int) noex ;
	int trackrel(cvoid *) noex ;
	int trackpresent(cvoid *) noex ;
	int trackout(ulong *) noex ;
	int mallstats(ucmemalloc_stats *) noex ;
	int iinit() noex ;
	int ifini() noex ;
	void atforkbefore() noex {
	    mx.lockbegin() ;
	} ;
	void atforkafter() noex {
	    mx.lockend() ;
	} ;
	constexpr ucmemalloc() noex {
	    init(this,ucmemallocmem_init) ;
	    fini(this,ucmemallocmem_fini) ;
	} ;
	~ucmemalloc() noex {
	    int		rs = ifini() ;
	    if (rs < 0) {
		ulogerror("ucmenalloc",rs,"dtor-fini") ;
	    }
	} ; /* end dtor */
	void rserr(int rs) noex {
	    if ((rs < 0) && (st.err_rs == 0)) st.err_rs = rs ;
	} ;
	void numoutmax() noex {
	    ulong 	out = (st.num_allocs - st.num_frees) ;
	    if (out > st.num_outmax) st.num_outmax = out ;
	} ;
    } ; /* end structure (ucmemalloc) */
}

extern "C" {
    int		ucmemalloc_init() noex ;
    int		ucmemalloc_fini() noex ;
}

extern "C" {
    static void	ucmemalloc_atforkbefore() noex ;
    static void	ucmemalloc_atforkafter() noex ;
    static void	ucmemalloc_exit() noex ;
}


/* forward references */


/* local variables */

static ucmemalloc	ucmemalloc_data ;

static sysval		pagesize(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int ucmemalloc_init() noex {
	return ucmemalloc_data.iinit() ;
}

int ucmemalloc_fini() noex {
	return ucmemalloc_data.ifini() ;
}

int uc_mallset(int cmd) noex {
	return ucmemalloc_data.mallset(cmd) ;
}

int uc_malloc(int sz,void *vp) noex {
	ucmemalloc	*uip = &ucmemalloc_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackmalloc(sz,vp) ;
	} else {
	    rs = uc_libmalloc(sz,vp) ;
	}
	if (rs >= 0) {
	    uip->st.num_allocs += 1 ;
	    uip->st.num_malloc += 1 ;
	    uip->numoutmax() ;
	}
	uip->rserr(rs) ;
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (uc_malloc) */

int uc_valloc(int sz,void *vp) noex {
	ucmemalloc	*uip = &ucmemalloc_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackmalloc(sz,vp) ; /* |valloc| tracked as |malloc| */
	} else {
	    rs = uc_libvalloc(sz,vp) ;
	}
	if (rs >= 0) {
	    uip->st.num_allocs += 1 ;
	    uip->st.num_valloc += 1 ;
	    uip->numoutmax() ;
	}
	uip->rserr(rs) ;
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (uc_valloc) */

int uc_calloc(int nelem,int esize,void *vp) noex {
	ucmemalloc	*uip = &ucmemalloc_data ;
	cint		sz = (nelem * esize) ;
	int		rs ;
	if ((rs = uc_malloc(sz,vp)) >= 0) {
	    memset(vp,0,sz) ;
	    uip->st.num_malloc -= 1 ;
	    uip->st.num_calloc += 1 ;
	}
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (uc_calloc) */

int uc_realloc(cvoid *cp,int sz,void *vp) noex {
	ucmemalloc	*uip = &ucmemalloc_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackrealloc(cp,sz,vp) ;
	} else {
	    rs = uc_librealloc(cp,sz,vp) ;
	}
	if (rs >= 0) {
	    uip->st.num_frees += 1 ;
	    uip->st.num_realloc += 1 ;
	}
	uip->rserr(rs) ;
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (uc_realloc) */

int uc_free(cvoid *vp) noex {
	ucmemalloc	*uip = &ucmemalloc_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackfree(vp) ;
	} else {
	    rs = uc_libfree(vp) ;
	}
	if (rs >= 0) {
	    uip->st.num_frees += 1 ;
	}
	uip->rserr(rs) ;
	return rs ;
}
/* end subroutine (uc_free) */

int uc_mallcount(ulong *rp) noex {
	return ucmemalloc_data.mallcount(rp) ;
}
/* end subroutine (uc_mallcount) */

int uc_mallpresent(cvoid *a) noex {
	return ucmemalloc_data.trackpresent(a) ;
}
/* end subroutine (uc_mallpresent) */

int uc_mallout(ulong *rp) noex {
	return ucmemalloc_data.trackout(rp) ;
}
/* end subroutine (uc_mallout) */

int uc_mallstats(ucmemalloc_stats *statp) noex {
	int		rs = SR_FAULT ;
	if (statp) {
	    rs = ucmemalloc_data.mallstats(statp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_mallstats) */

#ifdef	COMMENT

int ucmallreg_curbegin(ucmallreg_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (curp) {
	    rs = SR_NOMEM ;
	    if ((curp->mcp = new(nothrow) memtrack_iter) != nullptr) {
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucmallreg_curbegin) */

int ucmallreg_curend(ucmallreg_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (curp) {
	    rs = SR_OK ;
	    if (curp->mcp) {
		delete curp->mcp ;
		curp->mcp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucmallreg_curend) */

int ucmallreg_curenum(ucmallreg_cur *curp,ucmallreg_ent *rp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (curp && rp) {
	int		rsize = 0 ;
	ucmemalloc	*uip = &ucmemalloc_data ;
	if ((rs = ucmemalloc_init()) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
	            int		ci = (curp->i < 0) ? 0 : (curp->i + 1) ;
	            if (ci < uip->regs_total) {
	                while (ci < uip->regs_total) {
	                    if (uip->regs[ci].a) {
	                        rp->addr = uip->regs[ci].a ;
	                        rp->size = uip->regs[ci].size ;
	                        rsize = uip->regs[ci].size ;
	                        curp->i = ci ;
	                        break ;
	                    }
	                    ci += 1 ;
	                } /* end while */
	                if (ci >= uip->regs_total) rs = SR_NOTFOUND ;
	            } else {
	                rs = SR_NOTFOUND ;
		    }
	            rs1 = mx.lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	        rs1 = uc_forklockend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	} /* end if (non-null) */
	return (rs >= 0) ? rsize : rs ;
}
/* end subroutine (ucmallreg_curenum) */

#endif /* COMMENT */


/* local subroutines */

int ucmemalloc::iinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint		to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {			/* <- the money shot */
	        if ((rs = mx.create) >= 0) {
	            void_f	b = ucmemalloc_atforkbefore ;
	            void_f	a = ucmemalloc_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(ucmemalloc_exit)) >= 0) {
	                    finitdone = true ;
	                    f = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexpunge(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
	            if (rs < 0) {
	                mx.destroy() ;
		    }
	        } /* end if (ptm-create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        rs = 1 ;			/* <- OK ready */
	            }
	            return rs ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (ucmemalloc::iinit) */

int ucmemalloc::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    if (ftrack) {
	        rs1 = trackfinish() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = ucmemalloc_atforkbefore ;
	        void_f	a = ucmemalloc_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (initialization done) */
	return rs ;
}
/* end method (ucmemalloc::ifini) */

int ucmemalloc::mallcount(ulong *rp) noex {
	const ulong	out = (st.num_allocs = st.num_frees) ;
	if (rp) {
	    *rp = out ;
	}
	return int(out & INT_MAX) ;
}
/* end subroutine (ucmemalloc::mallcount) */

int ucmemalloc::mallset(int cmd) noex {
	int		rs ;
	if ((rs = init) >= 0) {
	    switch (cmd) {
	    case 0:
	        rs = trackfinish() ;
	        break ;
	    case 1:
	        rs = trackstart(cmd) ;
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (init) */
	return rs ;
}
/* end subroutine (ucmemalloc::mallset) */

int ucmemalloc::trackstart(int opts) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		rv = 0 ;
	if (! ftrack) {
	    if ((rs = mx.lockbegin) >= 0) {
	        {
	            rs = trackstarter(opts) ;
		    rv = rs ;
	        }
	        rs1 = mx.lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (was not previously tracking) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (ucmemalloc::trackstart) */

int ucmemalloc::trackstarter(int) noex {
	int		rs = SR_OK ;
	if ((st.err_rs >= 0) && (! ftrack)) {
	    rs = mt.start() ;
	    rserr(rs) ;
	} /* end if */
	return rs ;
}
/* end subroutine (ucmemalloc_trackstarter) */

int ucmemalloc::trackfinish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ftrack) {
	    if ((rs = mx.lockbegin) >= 0) {
		{
	            ftrack = false ;
		    rs1 = mt.finish ;
		    if (rs >= 0) rs = rs1 ;
	        }
	        rs1 = mx.lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (was tracking) */
	return rs ;
}
/* end subroutine (ucmemalloc::trackfinish) */

int ucmemalloc::trackmalloc(int sz,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = pagesize) >= 0) {
	        const uintptr_t	ps = uintptr_t(rs) ;
	        const uintptr_t	ma = uintptr_t(vp) ;
	        rs = SR_BADFMT ;
	        if (ma >= ps) {		/* <- page-zero check */
	            if ((rs = uc_forklockbegin(-1)) >= 0) {
	                if ((rs = mx.lockbegin) >= 0) {
	                    if ((rs = uc_libmalloc(sz,vp)) >= 0) {
	                        const caddr_t	a = *((caddr_t *)vp) ;
				rv = rs ;
	                        rs = trackreg(a,sz) ;
	                    }
	                    rs1 = mx.lockend ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (mutex) */
	                rs1 = uc_forklockend() ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (forklock) */
	        } /* end if (ok) */
	    } /* end if (pagesize) */
	} /* end if (init) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (ucmemalloc::trackmalloc) */

int ucmemalloc::trackrealloc(cvoid *cp,int sz,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
	            if ((rs = uc_librealloc(cp,sz,vp)) >= 0) {
	                caddr_t	a = *((caddr_t *) vp) ;
			rv = rs ;
			{
	                    rs1 = trackrel(cp) ;
	                    if (rs >= 0) rs = rs1 ;
			}
			{
	                    rs1 = trackreg(a,sz) ;
	                    if (rs >= 0) rs = rs1 ;
			}
	            } /* end if (librealloc) */
	            rs1 = mx.lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	        rs1 = uc_forklockend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (ucmemalloc::trackrealloc) */

int ucmemalloc::trackfree(cvoid *cp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
	            if ((rs = trackrel(cp)) >= 0) {
	                rs = uc_libfree(cp) ;
			rv = rs ;
	            }
	            rs1 = mx.lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	        rs1 = uc_forklockend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (ucmemalloc::trackfree) */

int ucmemalloc::trackreg(cvoid *a,int sz) noex {
	int		rs = SR_OK ;
	if (ftrack) {
	    rs = mt.ins(a,sz) ;
	} /* end if */
	return rs ;
}
/* end method (ucmemalloc::trackreg) */

int ucmemalloc::trackrel(cvoid *a) noex {
	int		rs = SR_OK ;
	if (ftrack) {
	    rs = mt.rem(a) ;
	} /* end if */
	return rs ;
}
/* end method (ucmemalloc::trackrel) */

int ucmemalloc::trackpresent(cvoid *cp) noex {
	int		rs ;
	int		rs1 ;
	int		sz = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
		    rs = SR_NOTOPEN ;
	            if (ftrack) {
	                rs = mt.present(cp) ;
	                sz = rs ;
	            }
	            rs1 = mx.lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	        rs1 = uc_forklockend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (ucmemalloc::trackpresent) */

int ucmemalloc::trackout(ulong *rp) noex {
	if (rp) {
	    *rp = st.out_size ;
	}
	return int(st.out_size & INT_MAX) ;
}
/* end subroutine (ucmemalloc::trackout) */

int ucmemalloc::mallstats(ucmemalloc_stats *statp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = mx.lockbegin) >= 0) {
		{
		    *statp = st ;
	    	    rv = int(st.out_size & INT_MAX) ;
		}
	        rs1 = mx.lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (init) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (ucmemalloc::mallstats) */

static void ucmemalloc_atforkbefore() noex {
	ucmemalloc_data.atforkbefore() ;
}
/* end subroutine (ucmemalloc_atforkbefore) */

static void ucmemalloc_atforkafter() noex {
	ucmemalloc_data.atforkafter() ;
}
/* end subroutine (ucmemalloc_atforkafter) */

static void ucmemalloc_exit() noex {
	/* ucmemalloc_data.fvoid = true ; */
}
/* end subroutine (ucmemalloc_atforkafter) */

ucmemalloc_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	switch (w) {
	case ucmemallocmem_init:
	    rs = op->iinit() ;
	    break ;
	case ucmemallocmem_fini:
	    rs = op->ifini() ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (ucmemalloc_co::operator) */


