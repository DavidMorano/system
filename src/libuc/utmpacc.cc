/* utmpacc */
/* lang=C++20 */

/* UNIX® UTMP access management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2018-11-21, David A.D. Morano
	I enhanced the caching of different entry types.

*/

/* Copyright © 2000,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module serves as a per-process cache for UNIX® UTMP
	information.

	Since we are basically dealing with global data, we need
	to make the establishment of it multi-thread safe.  We also
	want fork safety.  Yes, we want everything, including cleanup
	on module unloading (since, yes, we could all be inside a
	loadable and unloadble module!).  For these purposes we
	employ the basic (and not so basic) means of accomplishing
	this.  See the code for our various machinations.

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
	-		count of requested user types

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>		/* for |getsid(3c)| */
#include	<cstring>
#include	<atomic>
#include	<usystem.h>
#include	<usupport.h>
#include	<timewatch.hh>
#include	<ptm.h>
#include	<ptc.h>
#include	<sigblocker.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<syshasutmpx.h>
#include	<filebuf.h>

#include	"utmpacc.h"


/* local defines */

#define	UTMPACC_ITEM	struct utmpacc_i
#define	UTMPACC_NTYPES	4		/* number of process types */

/* intervals (seconds) */
#define	UTMPACC_INTBOOT		(5*3600)
#define	UTMPACC_INTRUNLEVEL	5
#define	UTMPACC_INTUSERS	10
#define	UTMPACC_INTENT		10
#define	UTMPACC_TOCAP		(5*60)

#define	ENT			UTMPACC_ENT


/* local namespaces */

using std::atomic_int ;


/* local typedefs */


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
    struct utmpacc {
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
	atomic_int	waiters ;
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
	int iinit() noex ;
	int ifini() noex ;
	int icapbegin(int) noex ;
	int icapend() noex ;
	int ibegin() noex ;
	int iend() noex ;
	int boottime(time_t *) noex ;
	int irunlevel() noex ;
	int iusers(int) noex ;
	int entsid(ENT *,char *,int,pid_t) noex ;
	int stats(utmpacc_sb *) noex ;
	int extract(int) noex ;
	int scan(time_t) noex ;
	int getentsid(time_t,ENT *,char *,int,pid_t) noex ;
	int getextract(int) noex ;
    } ; /* end struct (utmpacc) */
}


/* forward references */

extern "C" {
    static void	utmpacc_atforkbefore() noex ;
    static void	utmpacc_atforkafter() noex ;
    static void	utmpacc_exit() noex ;
}

static int utmpx_eterm(const utmpx *) noex ;


/* local variables */

static utmpacc		utmpacc_data ;


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
	return utmpacc_data.entsid(uep,uebuf,uelen,sid) ;
}

int utmpacc_stats(utmpacc_sb *usp) noex {
	return utmpacc_data.stats(usp) ;
}

int utmpacc_extract(int fd) noex {
	return utmpacc_data.extract(fd) ;
}


/* local subroutines */

int utmpacc_co::callout(int v) noex {
	int	rs = SR_BUGCHECK ;
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
	return rs ;
}
/* end method (utmpacc_co::callout) */

int utmpacc::iinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
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
	                    if (rs < 0)
	                        uc_atforkexpunge(b,a,a) ;
	                } /* end if (uc_atfork) */
	                if (rs < 0)
	                    cv.destroy() ;
	            } /* end if (ptc_create) */
	            if (rs < 0)
	                mx.destroy() ;
	        } /* end if (ptm_create) */
	        if (rs < 0)
	            finit = false ;
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
	    sigblocker	b ;
	    *tp = 0 ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin(-1)) >= 0) {
		        if ((rs = begin) >= 0) {
	                    const time_t	dt = time(NULL) ;
	                    const int		to = UTMPACC_INTBOOT ;
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
	sigblocker	b ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = b.start) >= 0) {
	    if ((rs = init) >= 0) {
	        if ((rs = capbegin(-1)) >= 0) {
		    if ((rs = begin) >= 0) {
	                const time_t	dt = time(NULL) ;
	                const int	to = UTMPACC_INTRUNLEVEL ;
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
	    sigblocker	b ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin(-1)) >= 0) {
		        if ((rs = begin) >= 0) {
	                    const time_t	dt = time(NULL) ;
	                    const int		to = UTMPACC_INTUSERS ;
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
/* end method (utmpacc::users) */

int utmpacc::entsid(ENT *uep,char *uebuf,int uelen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && uebuf) {
	    sigblocker	b ;
	    memclear(uep) ;
	    uebuf[0] = '\0' ;
	    if (sid <= 0) sid = getsid(0) ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin(-1)) >= 0) {
		        if ((rs = begin) >= 0) {	
	                    const time_t	dt = time(NULL) ;
			    int			ul = uelen ;
			    char		*up = uebuf ;
	                    rs = getentsid(dt,uep,up,ul,sid) ;
	                } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	    if ((rs >= 0) && (uep->line == NULL)) rs = SR_NOTFOUND ;
	} /* end if (non-null) */
	return rs ;
}
/* end method (utmpacc::entsid) */

int utmpacc::stats(utmpacc_sb *usp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		mu = 0 ;
	if (usp) {
	    sigblocker	b ;
	    memclear(usp) ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin(-1)) >= 0) {
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
	int		len = 0 ;
	if (fd >= 0) {
	    sigblocker	b ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init) >= 0) {
	            if ((rs = capbegin(-1)) >= 0) {
		        if ((rs = begin) >= 0) {
			    rs = getextract(fd) ;
			    len = rs ;
	                } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (valid) */
	return (rs >= 0) ? len : rs ;
}
/* end method (utmpacc::extract) */

int utmpacc::scan(time_t dt) noex {
	const utmpx	*up ;
	int		rs = SR_OK ;
	int		nu = 0 ;
	int		ne = 0 ;
	memclear(nusers,(sizeof(UTMPACC_ITEM) * utxproctype_overlast)) ;
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    int		pt = -1 ;
	    switch (up->ut_type) {
	    case UTMPACC_TRUNLEVEL:
	        {
	            cint	eterm = utmpx_eterm(up) ;
	            rlevel.v = MKCHAR(eterm) ;
	            rlevel.t = dt ;
	        }
		break ;
	    case UTMPACC_TBOOTTIME:
		{
	            btime.v = up->ut_tv.tv_sec ;
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
	}
	return rs ;
}
/* end method (utmpacc::scan) */

int utmpacc::getentsid(time_t dt,ENT *uep,
		char *uebuf,int uelen,pid_t sid) noex {
	const utmpx	*up ;
	int		rs = SR_OK ;
	int		ffound = false ;
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    int		pt = -1 ;
	    switch (up->ut_type) {
	    case UTMPACC_TRUNLEVEL:
	        {
	            cint	eterm = utmpx_eterm(up) ;
	            rlevel.v = MKCHAR(eterm) ;
	            rlevel.t = dt ;
	        }
		break ;
	    case UTMPACC_TBOOTTIME:
		{
	            btime.v = up->ut_tv.tv_sec ;
	            btime.t = dt ;
	        }
		break ;
	    case UTMPACC_TPROCUSER:
	            if (up->ut_user[0] != '\0') {
		    pt = utxproctype_user ;
	                if (up->ut_pid == sid)  {
			    ffound = true ;
	                    rs = utmpaccent_load(uep,uebuf,uelen,up) ;
	                }
	            } /* end if (non-nul) */
		break ;
	    case UTMPACC_TPROCINIT:
		pt = utxproctype_init ;
		break ;
	    case UTMPACC_TPROCLOGIN:
		pt = utxproctype_login ;
		break ;
	    case UTMPACC_TPROCDEAD:
		pt = utxproctype_dead ;
		break ;
	    } /* end switch (UTMP entry types) */
	        if ((pt >= 0) && (up->ut_user[0] != '\0')) {
	            nusers[pt].v += 1 ;
		} /* end if */
	    if (rs < 0) break ;
	} /* end while (reading UTMPX entries) */
	endutxent() ;
	if (rs >= 0) {
	    for (int i = 0 ; i < utxproctype_overlast ; i += 1) {
	        nusers[i].t = dt ;
	    }
	}
	return (rs >= 0) ? ffound : rs ;
}
/* end method (utmpacc::getentsid) */

int utmpacc::getextract(int fd) noex {
	filebuf		fb ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = filebuf_start(&fb,fd,0z,0,0)) >= 0) {
	    static constexpr int	ul = sizeof(utmpx) ;
	    const utmpx	*up ;
	    setutxent() ;
	    while ((up = getutxent()) != nullptr) {
		rs = filebuf_write(&fb,up,ul) ;
	        if (rs < 0) break ;
	    } /* end while (reading UTMPX entries) */
	    endutxent() ;
	    rs1 = filebuf_finish(&fb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filebuf) */
	return (rs >= 0) ? len : rs ;
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
	cint	rs = utmpacc_fini() ;
	if (rs < 0) {
	    ulogerror("utmpacc",rs,"exit-fini") ;
	}
}
/* end subroutine (utmpacc_exit) */

/* special local subroutines (depending on OS capability) */

#if	defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0)

static int utmpx_eterm(const utmpx *up) noex {
	return up->ut_exit.e_termination ;
}

#else /* defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0) */

static int utmpx_eterm(const utmpx *) noex {
	return 0 ;
}

#endif /* defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0) */


