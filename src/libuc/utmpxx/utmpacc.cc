/* utmpacc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® UTMP access management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A-D- Morano
	Originally written for Rightcore Network Services.

	= 2018-11-21, David A-D- Morano
	I enhanced the caching of different entry types.

*/

/* Copyright © 1998,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	utmpacc

	Description:
	This module serves as a per-process cache for UNIX® UTMP
	information.  Since we are basically dealing with global
	data, we need to make the establishment of it multi-thread
	safe.  We also want fork safety.  Yes, we want everything,
	including cleanup on module unloading (since, yes, we could
	all be inside a loadable and unloadble module!).  For these
	purposes we employ the basic (and not so basic) means of
	accomplishing this.  See the code for our various machinations.
	This facility is thread-safe, and async-signal safe..

	+ descriptions

	Name:
	utmpacc_users

	Synopsis:
	int utmpacc_users(int w) noex

	Arguments:
	w		which user logins to count:
				0	normal users only
				1	login gettys only
				2	init procs only

	Returns:
	>=0		count of requested user types
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>		/* for |getsid(3c)| */
#include	<utmpx.h>
#include	<climits>
#include	<csignal>		/* |sig_atomic_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<atomic>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<timewatch.hh>
#include	<syshasutmpx.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<sigblocker.h>
#include	<opentmp.h>
#include	<filemap.h>
#include	<filer.h>
#include	<snx.h>
#include	<strnxcmp.h>		/* <- for |strnncmp(3uc)| */
#include	<mkchar.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"utmpacc.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;

/* local defines */
#define	UTMPACC_ITEM		struct utmpacc_i
#define	UTMPACC_NTYPES		4	/* number of process types */
/* intervals (seconds) */
#define	UTMPACC_INTBOOT		(5*3600)
#define	UTMPACC_INTRUNLEVEL	5
#define	UTMPACC_INTUSERS	10
#define	UTMPACC_INTENT		10
#define	UTMPACC_TOCAP		(5*60)

#define	ENT			utmpaccent
#define	ARG			utmpacc_arg

#define	UTMPACC_ENTLENMULT	4	/* entry-legnth multiplier */

#ifndef	TERMBUFLEN
#define	TERMBUFLEN		256	/* terminal-device buffer length */
#endif

#ifndef	DEVPREFIX
#define	DEVPREFIX	"/dev/"
#endif


/* imported namespaces */

using std::atomic_int ;			/* type */
using libuc::libmem ;


/* local typedefs */

typedef volatile sig_atomic_t	vint ;


/* external subroutines */


/* local structures */

namespace {
    struct utmpacc_i {
	time_t		t ;		/* create-time */
	uint		v ;		/* value */
    } ;
    enum utmpaccmems {
	utmpaccmem_init,
	utmpaccmem_fini,
	utmpaccmem_capbegin,
	utmpaccmem_capend,
	utmpaccmem_begin,
	utmpaccmem_end,
	utmpaccmem_runlevel,
	utmpaccmem_users,
	utmpaccmem_overlast
    } ;
    struct utmpacc ;
    struct utmpacc_co {
	utmpacc		*op = nullptr ;
	int		w = -1 ;
	void operator () (utmpacc *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int callout(int v) noex ;
	int operator () (int v = -1) noex {
	    return callout(v) ;
	} ;
	operator int () noex {
	    return callout(-1) ;
	} ;
    } ; /* end struct (utmpacc_co) */
    struct utmpacc_arg {
	ENT		*uep ;
	time_t		dt = 0 ;
	char		*uebuf ;
	int		uelen ;
	utmpacc_arg(time_t t,ENT *p,char *b,int l) noex {
	    dt = t ;
	    uep = p ;
	    uebuf = b ;
	    uelen = l ;
	} ; /* end ctor */
    } ; /* end struct (utmpacc_arg) */
    struct utmpacc {
	friend		utmpacc_co ;
	utmpacc_co	init ;
	utmpacc_co	fini ;
	utmpacc_co	capbegin ;
	utmpacc_co	capend ;
	utmpacc_co	begin ;
	utmpacc_co	end ;
	utmpacc_co	runlevel ;
	utmpacc_co	users ;
	ptm		mx ;		/* data mutex */
	ptc		cv ;		/* condition variable */
	UTMPACC_ITEM	btime ;
	UTMPACC_ITEM	rlevel ;
	UTMPACC_ITEM	nusers[utxproctype_overlast] ;
	int		maxusers ;
	int		maxents ;
	vint		waiters ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		fcapture ;
	utmpacc() noex {
	    init(this,utmpaccmem_init) ;
	    fini(this,utmpaccmem_fini) ;
	    capbegin(this,utmpaccmem_capbegin) ;
	    capend(this,utmpaccmem_capend) ;
	    begin(this,utmpaccmem_begin) ;
	    end(this,utmpaccmem_end) ;
	    runlevel(this,utmpaccmem_runlevel) ;
	    users(this,utmpaccmem_users) ;
	} ;
	int boottime(time_t *) noex ;
	int irunlevel() noex ;
	int iusers(int) noex ;
	int entsid(ARG *,pid_t) noex ;
	int entstat(ARG *,pid_t) noex ;
	int entline(ARG *,cchar *,int) noex ;
	int stats(utmpacc_sb *) noex ;
	int extract(int) noex ;
	int scan(time_t) noex ;
	int getentsid(ARG *,pid_t) noex ;
	int getentstat(ARG *,pid_t) noex ;
	int getentline(ARG *,cchar *,int) noex ;
	int getextract(int) noex ;
    private:
	int iinit() noex ;
	int ifini() noex ;
	int icapbegin(int) noex ;
	int icapend() noex ;
	int ibegin() noex ;
	int iend() noex ;
    } ; /* end struct (utmpacc) */
    struct vars {
	int		entbuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

extern "C" {
    static void	utmpacc_atforkbefore() noex ;
    static void	utmpacc_atforkafter() noex ;
    static void	utmpacc_exit() noex ;
}

static bool isourtype(UTMPX *up) noex {
	bool	f = false ;
	f = f || (up->ut_type == UTMPACC_TPROCINIT) ;
	f = f || (up->ut_type == UTMPACC_TPROCUSER) ;
	f = f || (up->ut_type == UTMPACC_TPROCLOGIN) ;
	return f ;
}

static int	utmpx_eterm(CUTMPX *) noex ;

static int	utmpaccent_loada(ARG *,CUTMPX *) noex ;


/* local variables */

[[maybe_unused]] constexpr int 		lid   = UTMPACCENT_LID ;
[[maybe_unused]] constexpr int 		luser = UTMPACCENT_LUSER ;
[[maybe_unused]] constexpr int 		lline = UTMPACCENT_LLINE ;
[[maybe_unused]] constexpr int 		lhost = UTMPACCENT_LHOST ;

static vars		var ;

static utmpacc		utmpacc_data ;


/* exported variables */

utmpacc_enter		utmpacc_entbuflen ;


/* exported subroutines */

int utmpacc_init() noex {
	return utmpacc_data.init ;
}

int utmpacc_fini() noex {
	return utmpacc_data.fini ;
}

int utmpacc_boottime(time_t *tp) noex {
	return utmpacc_data.boottime(tp) ;
}

int utmpacc_runlevel() noex {
	return utmpacc_data.runlevel ;
}

int utmpacc_users(int w) noex {
	return utmpacc_data.users(w) ;
}

int utmpacc_entsid(ENT *uep,char *uebuf,int uelen,pid_t sid) noex {
	utmpacc_arg	a(0,uep,uebuf,uelen) ;
	return utmpacc_data.entsid(&a,sid) ;
}

int utmpacc_entstat(ENT *uep,char *uebuf,int uelen,pid_t sid) noex {
	utmpacc_arg	a(0,uep,uebuf,uelen) ;
	return utmpacc_data.entstat(&a,sid) ;
}

int utmpacc_entline(ENT *uep,char *uebuf,int uelen,cchar *lp,int ll) noex {
	utmpacc_arg	a(0,uep,uebuf,uelen) ;
	return utmpacc_data.entline(&a,lp,ll) ;
}

int utmpacc_stats(utmpacc_sb *usp) noex {
	return utmpacc_data.stats(usp) ;
}

int utmpacc_extract(int fd) noex {
	return utmpacc_data.extract(fd) ;
}

struct utmpacc_icur {
	UTMPX		*utmpfentp ;
	filer		fb ;
	uint		magic ;
	int		fd ;
} ;

int utmpacc_curbegin(utmpacc_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (curp) {
	    cnullptr	np{} ;
	    cint	csz = szof(utmpacc_icur) ;
	    curp->icursor = nullptr ;
	    if (void *vp ; (rs = libmem.mall(csz,&vp)) >= 0) {
		cint		usz = szof(UTMPX) ;
	        utmpacc_icur	*icurp = (utmpacc_icur *) vp ;
		memclear(icurp) ;
		if ((rs = libmem.mall(usz,&vp)) >= 0) {
		    UTMPX	*ufp = (UTMPX *) vp ;
		    cint	of = (O_CLOEXEC | O_MINFD) ;
		    cmode	om = 0664 ;
		    if ((rs = opentmp(np,of,om)) >= 0) {
		        cint	fd = rs ;
			if ((rs = utmpacc_extract(fd)) >= 0) {
			    if ((rs = u_rewind(fd)) >= 0) {
		                filer	*fbp = &icurp->fb ;
		                if ((rs = fbp->start(fd,0z,0,0)) >= 0) {
			            icurp->utmpfentp = ufp ;
			            icurp->fd = fd ;
			            icurp->magic = UTMPACC_CURMAGIC ;
			            curp->icursor = icurp ;
		                } /* end if (filer_start) */
			    } /* end if (u_rewind) */
			} /* end if (utmpacc_extract) */
		        if (rs < 0) {
			    uc_close(fd) ;
		        }
		    } /* end if (opentmp) */
		    if (rs < 0) {
			libmem.free(ufp) ;
			icurp->utmpfentp = nullptr ;
		    }
		} /* end if (memory-allocation) */
		if (rs < 0) {
		    libmem.free(icurp) ;
		    curp->icursor = nullptr ;
		}
	    } /* end if (mempory-allocation) */
	} /* end if (non-null) */
	return rs ;
}

int utmpacc_curenum(utmpacc_cur *curp,utmpacc_ent *ep,char *eb,int el) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (curp && ep && eb) {
	    utmpacc_icur	*icurp = (utmpacc_icur *) curp->icursor ;
	    rs = SR_NOTOPEN ;
	    memclear(ep) ;
	    if (icurp) {
	        rs = SR_BADFMT ;
		if (icurp->magic == UTMPACC_CURMAGIC) {
		    cint	usz = szof(UTMPX) ;
		    filer	*fbp = &icurp->fb ;
		    UTMPX	*ufp = icurp->utmpfentp ;
		    if ((rs = fbp->read(ufp,usz,-1)) >= 0) {
			rs = utmpaccent_load(ep,eb,el,ufp) ;
		        len = rs ;
		    }
		} /* end if (cursor-magic) */
	    } /* end if (internal-cursor-pointer ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}

int utmpacc_curend(utmpacc_cur *curp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (curp) {
	    rs = SR_NOTOPEN ;
	    if (curp->icursor) {
	        utmpacc_icur	*icurp = (utmpacc_icur *) curp->icursor ;
		rs = SR_OK ;
		if (icurp->magic == UTMPACC_CURMAGIC) {
		    {
		        filer	*fbp = &icurp->fb ;
			rs1 = fbp->finish ;
			if (rs >= 0) rs = rs1 ;
		    }
		    if (icurp->fd >= 0) {
		        rs1 = u_close(icurp->fd) ;
		        if (rs >= 0) rs = rs1 ;
		        icurp->fd = -1 ;
		    }
		    if (icurp->utmpfentp) {
			rs1 = libmem.free(icurp->utmpfentp) ;
			if (rs >= 0) rs = rs1 ;
			icurp->utmpfentp = nullptr ;
		    }
		    icurp->magic = 0 ;
		} else {
		    rs = SR_BADFMT ;
		} /* end if (good cursor-magic) */
		{
		    rs1 = libmem.free(curp->icursor) ;
		    if (rs >= 0) rs = rs1 ;
		    curp->icursor = nullptr ;
		}
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
}


/* local subroutines */

int utmpacc_co::callout(int v) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case utmpaccmem_init:
	        rs = op->iinit() ;
	        break ;
	    case utmpaccmem_fini:
	        rs = op->ifini() ;
	        break ;
	    case utmpaccmem_capbegin:
	        rs = op->icapbegin(v) ;
	        break ;
	    case utmpaccmem_capend:
	        rs = op->icapend() ;
	        break ;
	    case utmpaccmem_begin:
	        rs = op->ibegin() ;
	        break ;
	    case utmpaccmem_end:
	        rs = op->iend() ;
	        break ;
	    case utmpaccmem_runlevel:
	        rs = op->irunlevel() ;
	        break ;
	    case utmpaccmem_users:
	        rs = op->iusers(v) ;
	        break ;
	    } /* end switch */
	} /* end if */
	return rs ;
}
/* end method (utmpacc_co::callout) */

int utmpacc::iinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            if ((rs = cv.create) >= 0) {
	                void_f	b = utmpacc_atforkbefore ;
	                void_f	a = utmpacc_atforkafter ;
	                if ((rs = uc_atforkrecord(b,a,a)) >= 0) {
	                    if ((rs = uc_atexit(utmpacc_exit)) >= 0) {
	                        f = true ;
	                        finitdone = true ;
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
	            int		rsl = SR_OK ;
	            if (!finit) {
		        rsl = SR_LOCKLOST ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            }
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;			/* <- time-watching */
	    } /* end if (initialization) */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end method (utmpacc::iinit) */

int utmpacc::ifini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        rs1 = end ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        void_f	b = utmpacc_atforkbefore ;
	        void_f	a = utmpacc_atforkafter ;
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
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end method (utmpacc::ifini) */

int utmpacc::icapbegin(int to) noex {
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
/* end method (utmpacc::icapbegin) */

int utmpacc::icapend() noex {
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
/* end method (utmpacc::icapend) */

int utmpacc::ibegin() noex {
	return SR_OK ;
}
/* end method (utmpacc::begin) */

int utmpacc::iend() noex {
	return SR_OK ;
}
/* end method (utmpacc::end) */

int utmpacc::boottime(time_t *tp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tp) {
	    *tp = 0 ;
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin) >= 0) {
		        if ((rs = begin) >= 0) {
	                    custime	dt = time(nullptr) ;
	                    cint	to = UTMPACC_INTBOOT ;
	                    if ((dt - btime.t) >= to) {
	                        rs = scan(dt) ;
	                    } /* end if (timed-out) */
		        } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	            if (rs >= 0) *tp = btime.v ;
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblocker) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (utmpacc::boottime) */

int utmpacc::irunlevel() noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) {
	    if ((rs = init) >= 0) {
	        if ((rs = capbegin) >= 0) {
		    if ((rs = begin) >= 0) {
	                custime		dt = time(nullptr) ;
	                cint		to = UTMPACC_INTRUNLEVEL ;
	                if ((dt - rlevel.t) >= to) {
	                    rs = scan(dt) ;
	                } /* end if */
	                if (rs >= 0) n = rlevel.v ;
		    } /* end if */
	            rs1 = capend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (capture-exclusion) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? n : rs ;
}
/* end method (utmpacc::irunlevel) */

int utmpacc::iusers(int w) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	int		n = 0 ;
	if ((w >= 0) && (w < UTMPACC_NTYPES)) {
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin) >= 0) {
		        if ((rs = begin) >= 0) {
	                    custime	dt = time(nullptr) ;
	                    cint	to = UTMPACC_INTUSERS ;
	                    if ((dt - nusers[w].t) >= to) {
	                        rs = scan(dt) ;
	                    } /* end if */
	                    if (rs >= 0) n = nusers[w].v ;
		        } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblocker) */
	} /* end if (valid) */
	return (rs >= 0) ? n : rs ;
}
/* end method (utmpacc::iusers) */

int utmpacc::entsid(ARG *ap,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ffound = false ;
	if (ap && ap->uep && ap->uebuf) {
	    memclear(ap->uep) ;
	    ap->uebuf[0] = '\0' ;
	    if (sid <= 0) sid = getsid(0) ;
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin) >= 0) {
		        if ((rs = begin) >= 0) {	
	                    ap->dt = time(nullptr) ;
	                    rs = getentsid(ap,sid) ;
			    ffound = rs ;
	                } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	    if ((rs >= 0) && (ap->uep->line == nullptr)) rs = SR_NOTFOUND ;
	} /* end if (non-null) */
	return (rs >= 0) ? ffound : rs ;
}
/* end method (utmpacc::entsid) */

int utmpacc::entstat(ARG *ap,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ffound = false ;
	if (ap && ap->uep && ap->uebuf) {
	    memclear(ap->uep) ;
	    ap->uebuf[0] = '\0' ;
	    if (sid <= 0) sid = getsid(0) ;
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin) >= 0) {
		        if ((rs = begin) >= 0) {	
	                    ap->dt = time(nullptr) ;
	                    rs = getentstat(ap,sid) ;
			    ffound = rs ;
	                } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	    if ((rs >= 0) && (ap->uep->line == nullptr)) rs = SR_NOTFOUND ;
	} /* end if (non-null) */
	return (rs >= 0) ? ffound : rs ;
}
/* end method (utmpacc::entstat) */

int utmpacc::entline(ARG *ap,cchar *lp,int ll) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ffound = false ;
	if (ap && ap->uep && ap->uebuf && lp) {
	    memclear(ap->uep) ;
	    ap->uebuf[0] = '\0' ;
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin) >= 0) {
		        if ((rs = begin) >= 0) {	
	                    ap->dt = time(nullptr) ;
	                    rs = getentline(ap,lp,ll) ;
			    ffound = rs ;
	                } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	    if ((rs >= 0) && (ap->uep->line == nullptr)) rs = SR_NOTFOUND ;
	} /* end if (non-null) */
	return (rs >= 0) ? ffound : rs ;
}
/* end method (utmpacc::entline) */

int utmpacc::stats(utmpacc_sb *usp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		mu = 0 ;
	if (usp) {
	    memclear(usp) ;
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin) >= 0) {
		        if ((rs = begin) >= 0) {
	                    usp->maxusers = maxusers ;
	                    usp->maxents = maxents ;
			    mu = maxusers ;
	                } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (non-null) */
	return (rs >= 0) ? mu : rs ;
}
/* end method (utmpacc::stats) */

int utmpacc::extract(int fd) noex {
	int		rs = SR_BADF ;
	int		rs1 ;
	int		wlen = 0 ;
	if (fd >= 0) {
	    if (sigblocker b ; (rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin) >= 0) {
		        if ((rs = begin) >= 0) {
			    {
			        rs = getextract(fd) ;
			        wlen = rs ;
			    }
			    rs1 = u_rewind(fd) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (valid) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (utmpacc::extract) */

int utmpacc::scan(time_t dt) noex {
	CUTMPX		*up ;
	int		rs = SR_OK ;
	int		nu = 0 ;
	int		ne = 0 ;
	memclear(nusers,(szof(UTMPACC_ITEM) * utxproctype_overlast)) ;
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    int		pt = -1 ;
	    switch (up->ut_type) {
	    case UTMPACC_TRUNLEVEL:
	        {
	            cint	eterm = utmpx_eterm(up) ;
	            rlevel.v = mkchar(eterm) ;
	            rlevel.t = dt ;
	        }
		break ;
	    case UTMPACC_TBOOTTIME:
		{
	            btime.v = uint(up->ut_tv.tv_sec) ;
	            btime.t = dt ;
	        }
		break ;
	    case UTMPACC_TPROCINIT:
		pt = utxproctype_init ;
		break ;
	    case UTMPACC_TPROCLOGIN:
		pt = utxproctype_login ;
		break ;
	    case UTMPACC_TPROCUSER:
		nu += 1 ;
		pt = utxproctype_user ;
		break ;
	    case UTMPACC_TPROCDEAD:
		pt = utxproctype_dead ;
		break ;
	    } /* end switch */
	    if ((pt >= 0) && (up->ut_user[0] != '\0')) {
	        nusers[pt].v += 1 ;
	    } /* end if */
	    ne += 1 ;
	} /* end while */
	if (rs >= 0) {
	    for (int i = 0 ; i < utxproctype_overlast ; i += 1) {
	        nusers[i].t = dt ;
	    }
	    if (nu > maxusers) maxusers = nu ;
	    if (ne > maxents) maxents = ne ;
	} /* end if (ok) */
	return rs ;
}
/* end method (utmpacc::scan) */

int utmpacc::getentsid(ARG *ap,pid_t sid) noex {
	CUTMPX		*up ;
	int		rs = SR_OK ;
	int		ffound = false ;
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    switch (up->ut_type) {
	    case UTMPACC_TRUNLEVEL:
	        {
	            cint	eterm = utmpx_eterm(up) ;
	            rlevel.v = mkchar(eterm) ;
	            rlevel.t = ap->dt ;
	        }
		break ;
	    case UTMPACC_TBOOTTIME:
		{
	            btime.v = uint(up->ut_tv.tv_sec) ;
	            btime.t = ap->dt ;
	        }
		break ;
	    case UTMPACC_TPROCINIT:
	    case UTMPACC_TPROCUSER:
	    case UTMPACC_TPROCLOGIN:
	        if (up->ut_user[0] != '\0') {
	            if (up->ut_pid == sid)  {
			ffound = true ;
			rs = utmpaccent_loada(ap,up) ;
	            }
	        } /* end if (non-nul) */
	    } /* end switch (UTMP entry types) */
	    if (ffound) break ;
	    if (rs < 0) break ;
	} /* end while (reading UTMPX entries) */
	endutxent() ;
	return (rs >= 0) ? ffound : rs ;
}
/* end method (utmpacc::getentsid) */

int utmpacc::getentstat(ARG *ap,pid_t sid) noex {
	UTMPX		*up ;
	cint		tlen = TERMBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		ffound = false ;
	cchar		*devprefix = DEVPREFIX ;
	char		tbuf[tlen+1] ;
	if ((rs = sncpy(tbuf,tlen,devprefix)) >= 0) {
	    cint	tl = rs ;
	    setutxent() ;
	    while ((up = getutxent()) != nullptr) {
	       if (isourtype(up)) {
		    cint	ll = lline ;
		    cchar	*lp = up->ut_line ;
	            if ((rs = snaddw(tbuf,tlen,tl,lp,ll)) >= 0) {
			cint	of = (O_RDONLY | O_NOCTTY) ;
			cmode	om = 0666 ;
			if ((rs = u_open(tbuf,of,om)) >= 0) {
			    cint	fd = rs ;
			    if ((rs = uc_tcgetsid(fd)) >= 0) {
    				if (sid == rs) {
				    ffound = true ;
				    rs = utmpaccent_loada(ap,up) ;
				}
			    } else if (isNotTerm(rs)) {
				rs = SR_OK ;
			    } /* end if (uc_tcgetsid) */
			    rs1 = u_close(fd) ;
			    if (rs >= 0) rs = rs1 ;
			} else if (isNotAccess(rs)) {
			    rs = SR_OK ;
			}
		    } /* end if (snadd) */
	       } /* end if (our-type) */
	    } /* end while */
	} /* end if (sncpy) */
	return (rs >= 0) ? ffound : rs ;
}
/* end method (utmpacc::getentstat) */

int utmpacc::getentline(ARG *ap,cchar *lp,int ll) noex {
	CUTMPX		*up ;
	int		rs = SR_OK ;
	int		ffound = false ;
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    switch (up->ut_type) {
	    case UTMPACC_TRUNLEVEL:
	        {
	            cint	eterm = utmpx_eterm(up) ;
	            rlevel.v = mkchar(eterm) ;
	            rlevel.t = ap->dt ;
	        }
		break ;
	    case UTMPACC_TBOOTTIME:
		{
	            btime.v = uint(up->ut_tv.tv_sec) ;
	            btime.t = ap->dt ;
	        }
		break ;
	    case UTMPACC_TPROCINIT:
	    case UTMPACC_TPROCLOGIN:
	    case UTMPACC_TPROCUSER:
	        if (up->ut_user[0] != '\0') {
		    if (ap->uep->line == nullptr) {
	                if (strnncmp(up->ut_line,lline,lp,ll) == 0)  {
			    ffound = true ;
			    rs = utmpaccent_loada(ap,up) ;
		        } /* end if (matched) */
	            } /* end if (line-field was empty) */
	        } /* end if (non-nul) */
		break ;
	    } /* end switch (UTMP entry types) */
	    if (ffound) break ;
	    if (rs < 0) break ;
	} /* end while (reading UTMPX entries) */
	endutxent() ;
	return (rs >= 0) ? ffound : rs ;
}
/* end method (utmpacc::getentline) */

int utmpacc::getextract(int fd) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (filer fb ; (rs = fb.start(fd,0z,0,0)) >= 0) {
	    {
	        cint	ul = szof(UTMPX) ;
	        CUTMPX	*up ;
	        setutxent() ;
	        while ((up = getutxent()) != nullptr) {
		    rs = fb.write(up,ul) ;
		    wlen += rs ;
	            if (rs < 0) break ;
	        } /* end while (reading UTMPX entries) */
	        endutxent() ;
	    } /* end block */
	    rs1 = fb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (utmpacc::getextract) */

static void utmpacc_atforkbefore() noex {
	utmpacc_data.capbegin() ;
}
/* end subroutine (utmpacc_atforkbefore) */

static void utmpacc_atforkafter() noex {
	utmpacc_data.capend() ;
}
/* end subroutine (utmpacc_atforkafter) */

static void utmpacc_exit() noex {
	if (cint rs = utmpacc_fini() ; rs < 0) {
	    ulogerror("utmpacc",rs,"exit-fini") ;
	}
}
/* end subroutine (utmpacc_exit) */

static int utmpaccent_loada(ARG *ap,CUTMPX *up) noex {
	return utmpaccent_load(ap->uep,ap->uebuf,ap->uelen,up) ;
}

/* special local subroutines (depending on OS capability) */

#if	defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0)

static int utmpx_eterm(CUTMPX *up) noex {
	return up->ut_exit.e_termination ;
}

#else /* defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0) */

static int utmpx_eterm(CUTMPX *) noex {
	return 0 ;
}

#endif /* defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0) */

utmpacc_enter::operator int () noex {
    	static cint	rsv = var ;
    	int		rs ;
	if ((rs = rsv) >= 0) {
	    rs = var.entbuflen ;
	}
    	return rs ;
}

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_nn)) >= 0) {
	    entbuflen = szof(utmpx) ;
	}
	return rs ;
}
/* end method (vars::operator) */


