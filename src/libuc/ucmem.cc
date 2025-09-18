/* ucmem SUPPORT (3uc) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* memory allocation facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This code was originally written.

	= 2001-02-01, David A­D­ Morano
	I expanded the storage table size somewhat for larger
	programs.  We are handling larger amounts of data now-a-days!

	= 2011-04-12, David A­D­ Morano
	I made a few changes for the new decade.  Ya, time flies when
	one is coding day and night.  The changes are:

	1. I removed support for linking in the |libmalloc| debugging
	library (was supplied by some vendor).  Find other ways to
	debug if you have to.  There was not really much to that in
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
	|ucmem_stats|.  I coded this sort of thing in other
	places I have worked, but not yet here in my own code
	(below).  I generally do not have access to code that I
	create for others (proprietary copyright and all of that).
	But I tend to follow my own recollection when implementing
	certain features over and over again over some decades.

*/

/* Copyright © 1998,2001,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_mem{xx}

	Description:
	This is the friendly version of the standrd |malloc(3c)|
	subroutine and its various friends.  These subroutines are
	not interpositioned on the standard versions but rather have
	their own symbol names.  The main feature of these subroutines
	is to provide an optional tracking of the allocated memory
	usage.

	API:
	uc_mallset()	turns on memory tracking
	uc_malloc()	analogous to the standard |malloc(3c)|
	uc_valloc()	analogous to the standard |valloc(3c)|
	uc_calloc()	analogous to the standard |calloc(3c)|
	uc_realloc()	analogous to the standard |realloc(3c)|
	uc_free()	analogous to the standard |free(3c)|
	uc_mallpresent(cvoid *a) noex
	uc_mallout(ulong *rp) noex
	uc_mallstats(ucmem_stats *statp) noex

	Notes: 
	Because these subroutines are used *everywhere* they
	really have to be MT-safe (ya, for real).  They are not by
	themselves Async-Signal-Safe, but you can do that yourself
	if you want to.  Oh, they are Fork-Safe also.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<functional>		/* |mem_fn(3c++)| */
#include	<uclibsubs.h>
#include	<uclibmem.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"ucmem.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"
#pragma		GCC dependency		"mod/addrset.ccm"

import libutil ;			/* |memclear(3u)| */
import ulibvals ;			/* |ulibval.pagesz(3u)| */
import addrset ;

/* local defines */


/* imported namespaces */

using libuc::libmems ;			/* type */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (libmems::*alloc_m)(int,void *) noex ;
}

/* external subroutines */


/* external variables */


/* local structures */

namespace {
    enum submgrmems {
	submgrmem_init,
	submgrmem_fini,
	submgrmem_overlast
    } ;
    struct submgr ;
    struct submgr_co {
	submgr		*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (submgr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ; /* end struct (submgr_co) */
    typedef int (submgr::*submgr_m)(void *,int,void *) noex ;
    struct submgr {
	typedef ucmem_stats	statblock ;
	ptm		mx ;			/* object mutex */
	addrset		mt ;
	statblock	st ;
	submgr_co	init ;
	submgr_co	fini ;
	submgr_m	m ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		ftrack ;
	alloc_m		almem ;
	int mallcount(ulong *) noex ;
	int mallset(int) noex ;
	int trackstart(int) noex ;
	int trackstarter(int) noex ;
	int trackfinish() noex ;
	int trackcall(void *,int = 0,void * = nullptr) noex ;
	int trackmalloc(int,void *) noex ;
	int trackvalloc(int,void *) noex ;
	int trackrealloc(void *,int,void *) noex ;
	int trackfree(void *) noex ;
	int trackpresent(cvoid *) noex ;	/* track-present */
	int trackcurenum(ucmem_cur *,ucmem_ent *) noex ;
	int trackreg(cvoid *,int) noex ;	/* track-register */
	int trackrel(cvoid *) noex ;		/* track-release */
	int trackout(ulong *) noex ;
	int mallstats(ucmem_stats *) noex ;
	int callxalloc(void *,int,void *) noex ;
	int callrealloc(void *,int,void *) noex ;
	int callfree(void *,int,void *) noex ;
	int callpresent(void *,int,void *) noex ;
	int callcurenum(ucmem_cur *,ucmem_ent *) noex ;
	int iinit() noex ; 
	int ifini() noex ;
	void atforkbefore() noex {
	    mx.lockbegin() ;
	} ;
	void atforkafter() noex {
	    mx.lockend() ;
	} ;
	constexpr submgr() noex {
	    init	(this,submgrmem_init) ;
	    fini	(this,submgrmem_fini) ;
	} ;
	destruct submgr() noex {
	    if (cint rs = fini ; rs < 0) {
		ulogerror("ucmenalloc",rs,"dtor-fini") ;
	    }
	} ; /* end dtor */
	void rserr(int rs) noex {
	    if ((rs < 0) && (st.err_rs == 0)) {
		st.err_rs = rs ;
	    }
	} ;
	void numoutmax() noex {
	    ulong 	out = (st.num_allocs - st.num_frees) ;
	    if (out > st.num_outmax) st.num_outmax = out ;
	} ;
    } ; /* end structure (ucmem) */
} /* end namespace */

extern "C" {
    int		ucmem_init() noex ;
    int		ucmem_fini() noex ;
}

extern "C" {
    static void	submgr_atforkbefore() noex ;
    static void	submgr_atforkafter() noex ;
    static void	submgr_exit() noex ;
}


/* forward references */


/* local variables */

static submgr		submgr_data ;

static int		pagesz = ulibval.pagesz ;


/* exported variables */

namespace libuc {
    libuc::mems		mem ;
}

using libuc::mem ;


/* exported subroutines */

int ucmem_init() noex {
	return submgr_data.init ;
}

int ucmem_fini() noex {
	return submgr_data.fini ;
}

int uc_mallset(int cmd) noex {
    	return mem.mallset(cmd) ;
}

int uc_malloc(int sz,void *vp) noex {
    	return mem.malloc(sz,vp) ;
}

int uc_valloc(int sz,void *vp) noex {
    	return mem.valloc(sz,vp) ;
}

int uc_calloc(int ne,int esz,void *vp) noex {
    	return mem.calloc(ne,esz,vp) ;
}

int uc_realloc(void *cp,int sz,void *vp) noex {
    	return mem.ralloc(cp,sz,vp) ;
}

int uc_free(void *vp) noex {
    	return mem.free(vp) ;
}

int uc_mallcount(ulong *rp) noex {
    	return mem.mallcount(rp) ;
}

int uc_mallpresent(cvoid *a) noex {
    	return mem.mallpresent(a) ;
}

int uc_mallout(ulong *rp) noex {
    	return mem.mallout(rp) ;
}

int uc_mallstats(ucmem_stats *statp) noex {
    	return mem.mallstats(statp) ;
}

int ucmem_curbegin(ucmem_cur *curp) noex {
    	return mem.curbegin(curp) ;
}

int ucmem_curend(ucmem_cur *curp) noex {
    	return mem.curend(curp) ;
}

int ucmem_curenum(ucmem_cur *curp,ucmem_ent *rp) noex {
    	return mem.curenum(curp,rp) ;
}


/* local subroutines */

namespace libuc {
    int mems::strw(cchar *sp,int µsl,void *vp) noex {
	int		rs = SR_FAULT ;
	int		ml = 0 ; /* return-value */
	if (sp && vp) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
		if (char *bp ; (rs = mem.mall((sl + 1),&bp)) >= 0) {
		    cchar **rpp = ccharpp(vp) ;
		    ml = intconv(strwcpy(bp,sp,sl) - bp) ;
		    *rpp = bp ;
		} /* end if (memory-allocation) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? ml : rs ;
    } /* end method (mems::strw) */
    int mems::mall(int sz,void *vp) noex {
	submgr		*uip = &submgr_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackmalloc(sz,vp) ;
	} else {
	    rs = libmem.mall(sz,vp) ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end method (mems::mall) */
    int mems::vall(int sz,void *vp) noex {
	submgr		*uip = &submgr_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackvalloc(sz,vp) ;
	} else {
	    rs = libmem.vall(sz,vp) ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end method (mems::vall) */
    int mems::call(int ne,int esz,void *vp) noex {
	cint		sz = (ne * esz) ;
	int		rs ;
	if ((rs = mall(sz,vp)) >= 0) {
	    memclear(vp,sz) ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end method (mems::call) */
    int mems::rall(void *cp,int sz,void *vp) noex {
	submgr		*uip = &submgr_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackrealloc(cp,sz,vp) ;
	} else {
	    rs = libmem.rall(cp,sz,vp) ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end method (mems::rall) */
    int mems::free(void *vp) noex {
	submgr		*uip = &submgr_data ;
	int		rs ;
	if (uip->ftrack) {
	    rs = uip->trackfree(vp) ;
	} else {
	    rs = libmem.free(vp) ;
	}
	return rs ;
    } /* end method (mems::free) */
    int mems::mallset(int cmd) noex {
	return submgr_data.mallset(cmd) ;
    } /* end method (mems::mallset) */
    int mems::mallcount(ulong *rp) noex {
	return submgr_data.mallcount(rp) ;
    } /* end method (mems::mallcount) */
    int mems::mallout(ulong *rp) noex {
	return submgr_data.trackout(rp) ;
    } /* end method (mems::mallout) */
    int mems::mallstats(ucmem_stats *statp) noex {
	int		rs = SR_FAULT ;
	if (statp) {
	    rs = submgr_data.mallstats(statp) ;
	} /* end if (non-null) */
	return rs ;
    } /* end method (mems::_mallstats) */
    int mems::mallpresent(cvoid *a) noex {
	return submgr_data.trackpresent(a) ;
    } /* end method (mems::mallpresent) */
    int mems::curbegin(ucmem_cur *curp) noex {
    	cnothrow	nt{} ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (curp) {
	    rs = SR_NOMEM ;
	    if (addrset_cur *acp ; (acp = new(nt) addrset_cur) != np) {
	        addrset		*aop = &submgr_data.mt ;
		if ((rs = aop->curbegin(acp)) >= 0) {
		   curp->mcp = acp ;
		}
		if (rs < 0) {
		    delete acp ;
		}
	    } /* end if (new-addrset_cur) */
	} /* end if (non-null) */
	return rs ;
    } /* end method (mems::curbegin) */
    int mems::curend(ucmem_cur *curp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (curp) {
	    rs = SR_OK ;
	    if (curp->mcp) {
	        addrset		*aop = &submgr_data.mt ;
	        addrset_cur	*acp = (addrset_cur *) curp->mcp ;
		{
		    rs1 = aop->curend(acp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    delete acp ;
		}
		curp->mcp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end method (mems::curend) */
    int mems::curenum(ucmem_cur *curp,ucmem_ent *rp) noex {
	int		rs = SR_FAULT ;
	if (curp && rp) {
	    rs = submgr_data.trackcurenum(curp,rp) ;
	} /* end if (non-null) */
	return rs ;
    } /* end method (mems::curenum) */
} /* end namespace */

namespace libuc {
    int mems::malloc(int sz,void *vp) noex {
	return libuc::mem.mall(sz,vp) ;
    }
    int mems::valloc(int sz,void *vp) noex {
	return libuc::mem.vall(sz,vp) ;
    }
    int mems::calloc(int ne,int esz,void *vp) noex {
	return libuc::mem.call(ne,esz,vp) ;
    }
    int mems::ralloc(void *cp,int nsz,void *vp) noex {
	return libuc::mem.rall(cp,nsz,vp) ;
    }
} /* end namespace (libuc) */

int submgr::iinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {			/* <- the money shot */
	        if ((rs = mx.create) >= 0) {
	            void_f	b = submgr_atforkbefore ;
	            void_f	a = submgr_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
			void_f	e = submgr_exit ;
	                if ((rs = uc_atexit(e)) >= 0) {
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
	            int		rsl = SR_OK ;
	            if (!finit) {
		        rsl = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        rsl = 1 ;			/* <- OK ready */
	            }
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (submgr::iinit) */

int submgr::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    if (ftrack) {
	        rs1 = trackfinish() ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = submgr_atforkbefore ;
	        void_f	a = submgr_atforkafter ;
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
/* end method (submgr::ifini) */

int submgr::mallcount(ulong *rp) noex {
	const ulong	out = (st.num_allocs = st.num_frees) ;
	if (rp) {
	    *rp = out ;
	}
	return intsat(out) ;
}
/* end subroutine (submgr::mallcount) */

int submgr::mallset(int cmd) noex {
	int		rs ;
	if ((rs = init) >= 0) {
	    switch (cmd) {
	    case ucmallset_off:
	        rs = trackfinish() ;
	        break ;
	    case ucmallset_on:
	        rs = trackstart(cmd) ;
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (init) */
	return rs ;
}
/* end subroutine (submgr::mallset) */

int submgr::trackstart(int opts) noex {
	int		rs = SR_INPROGRESS ;
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
/* end subroutine (submgr::trackstart) */

int submgr::trackstarter(int) noex {
	int		rs = SR_OK ;
	if ((st.err_rs >= 0) && (! ftrack)) {
	    rs = mt.start() ;
	    rserr(rs) ;
	} /* end if */
	return rs ;
}
/* end subroutine (submgr_trackstarter) */

int submgr::trackfinish() noex {
	int		rs = SR_NOTOPEN ;
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
/* end subroutine (submgr::trackfinish) */

int submgr::trackcall(void *cp,int sz,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
		if ((rs = mx.lockbegin) >= 0) {
		    {
		        rs = (this->*m)(cp,sz,vp) ;
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
/* end subroutine (submgr::trackcall) */

int submgr::trackmalloc(int sz,void *vp) noex {
	m = &submgr::callxalloc ;
	almem = &libmems::mall ;
	return trackcall(nullptr,sz,vp) ;
}

int submgr::trackvalloc(int sz,void *vp) noex {
	m = &submgr::callxalloc ;
	almem = &libmems::vall ;
	return trackcall(nullptr,sz,vp) ;
}

int submgr::trackrealloc(void *cp,int sz,void *vp) noex {
	m = &submgr::callrealloc ;
	return trackcall(cp,sz,vp) ;
}

int submgr::trackfree(void *cp) noex {
	m = &submgr::callfree ;
	return trackcall(cp) ;
}

/* track-present (address-size) */
int submgr::trackpresent(cvoid *cp) noex {
    	void	*vp = cast_const<voidp>(cp) ;
	m = &submgr::callpresent ;
	return trackcall(vp) ;
}
/* end subroutine (submgr::trackpresent) */

int submgr::trackcurenum(ucmem_cur *curp,ucmem_ent *rp) noex {
	int		rs ;
	int		rs1 ;
	int		rsz = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
		    {
			rs = callcurenum(curp,rp) ;
			rsz = rs ;
		    }
	            rs1 = mx.lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	        rs1 = uc_forklockend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (forklock) */
	} /* end if (init) */
	return (rs >= 0) ? rsz : rs ;
}
/* end method (submgr::trackcurenum) */

int submgr::callxalloc(void *,int sz,void *vp) noex {
	int		rs ;
	int		rv = 0 ;
	if ((rs = pagesz) >= 0) {
	    const uintptr_t	ps = uintptr_t(rs) ;
	    const uintptr_t	ma = uintptr_t(vp) ;
	    rs = SR_BADFMT ;
	    if (ma >= ps) {		/* <- page-zero check */
	        if ((rs = (libmem.*almem)(sz,vp)) >= 0) {
		    const caddr_t	a = *((caddr_t *) vp) ;
		    rv = rs ;
		    rs = trackreg(a,sz) ;
		}
	    } /* end if (ok) */
	} /* end if (pagesz) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (submgr::trackmalloc) */

int submgr::callrealloc(void *cp,int sz,void *vp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = trackrel(cp)) >= 0) {
	    if ((rs = libmem.rall(cp,sz,vp)) >= 0) {
	        caddr_t	a = *((caddr_t *) vp) ;
	        rv = rs ;
	        {
		    rs1 = trackreg(a,sz) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (librealloc) */
	} /* end if (trackrel) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (submgr::callrealloc) */

int submgr::callfree(void *cp,int,void *) noex {
	int		rs ;
	int		rv = 0 ;
	if ((rs = trackrel(cp)) >= 0) {
	    rs = libmem.free(cp) ;
	    rv = rs ;
	}
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (submgr::callfree) */

/* track-present (address-size) */
int submgr::callpresent(void *cp,int,void *) noex {
	int		rs = SR_NOTOPEN ;
	int		sz = 0 ;
	if (ftrack) {
	    rs = mt.present(cp) ;
	    sz = rs ;
	}
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (submgr::callpresent) */

int submgr::callcurenum(ucmem_cur *curp,ucmem_ent *rp) noex {
	int		rs = SR_NOTOPEN ;
	int		rsz = 0 ;
	if (ftrack) {
	    addrset	*aop = &mt ;
	    addrset_cur	*acp = (addrset_cur *) curp->mcp ;
	    if (addrset_ent e{} ; (rs = aop->curenum(acp,&e)) >= 0) {
	        rp->addr = caddr_t(e.addr) ;
	        rp->asize = e.asize ;
	        rsz = intsat(e.asize) ;
	    } /* end if */
	} /* end if (tracking) */
	return (rs >= 0) ? rsz : rs ;
}
/* end subroutine (submgr::callcurenum) */

/* track-register (address-size) */
int submgr::trackreg(cvoid *a,int sz) noex {
	int		rs = SR_OK ;
	if (ftrack) {
	    rs = mt.ins(a,sz) ;
	} /* end if */
	return rs ;
}
/* end method (submgr::trackreg) */

/* track-release (address-size) */
int submgr::trackrel(cvoid *a) noex {
	int		rs = SR_OK ;
	if (ftrack) {
	    rs = mt.rem(a) ;
	} /* end if */
	return rs ;
}
/* end method (submgr::trackrel) */

int submgr::trackout(ulong *rp) noex {
	if (rp) {
	    *rp = st.out_size ;
	}
	return intsat(st.out_size) ;
}
/* end subroutine (submgr::trackout) */

int submgr::mallstats(ucmem_stats *statp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = init) >= 0) {
	    if ((rs = uc_forklockbegin(-1)) >= 0) {
	        if ((rs = mx.lockbegin) >= 0) {
		    {
		        *statp = st ;
	    	        rv = intsat(st.out_size) ;
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
/* end subroutine (submgr::mallstats) */

static void submgr_atforkbefore() noex {
	submgr_data.atforkbefore() ;
}
/* end subroutine (submgr_atforkbefore) */

static void submgr_atforkafter() noex {
	submgr_data.atforkafter() ;
}
/* end subroutine (submgr_atforkafter) */

static void submgr_exit() noex {
	/* submgr_data.fvoid = true ; */
}
/* end subroutine (submgr_atforkafter) */

submgr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case submgrmem_init:
	        rs = op->iinit() ;
	        break ;
	    case submgrmem_fini:
	        rs = op->ifini() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (submgr_co::operator) */


