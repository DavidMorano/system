/* utmpacc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® UTMP access management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

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
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX |getsid(3c)| */
#include	<utmpx.h>		/* POSIX */
#include	<csignal>		/* CSTD |sig_atomic_t| */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<atomic>		/* C++STD */
#include	<memory>		/* C++STD |destroy_at(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucfork.h>		/* LIBUC */
#include	<ucatfork.h>		/* LIBUC */
#include	<ucatexit.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<uctc.h>		/* LIBUC terminal-control */
#include	<timewatch.hh>		/* LIBU */
#include	<syshasutmpx.h>
#include	<ptm.h>			/* LIBU */
#include	<ptc.h>			/* LIBU */
#include	<sigblocker.h>		/* LIBU */
#include	<opentmp.h>		/* LIBUC */
#include	<filemap.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<strnxcmp.h>		/* LIBUC |strnncmp(3uc)| */
#include	<mkchar.h>		/* LIBU */
#include	<isoneof.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"utmpacc.h"
#include	"utmpaccent.h"		/* should be redundant */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */
#define	UTMPACC_NTYPES		4	/* number of process types */
/* intervals (seconds) */
#define	UTMPACC_INTBOOT		(5*3600)
#define	UTMPACC_INTRUNLEVEL	5
#define	UTMPACC_INTUSERS	10
#define	UTMPACC_INTENT		10
#define	UTMPACC_CAPTO		(5*60)	/* capture time-out */

#define	ENT			utmpacc_ent
#define	ARG			utmpacc_arg

#define	UTMPACC_ENTLENMULT	4	/* entry-legnth multiplier */

#ifndef	TERMBUFLEN
#define	TERMBUFLEN		256	/* terminal-device buffer length */
#endif

#ifndef	DEVPREFIX
#define	DEVPREFIX		"/dev/"
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::atomic_int ;			/* type */
using std::destroy_at ;			/* subroutine-template? */
using libuc::libmem ;			/* variable */


/* local typedefs */

typedef volatile sig_atomic_t	vint ;


/* external subroutines */


/* local structures */

namespace {
    struct utmpacc_item {
	time_t		t ;		/* create-time */
	uint		v ;		/* value */
    } ; /* end struct (utmpacc_item) */
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
    } ; /* end enum (utmpaccmems) */
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
	char		*uebuf ;
	time_t		dt = 0 ;
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
	ptm		omx ;		/* object data mutex */
	ptc		ocv ;		/* object condition variable */
	utmpacc_item	btime ;
	utmpacc_item	rlevel ;
	utmpacc_item	nusers[utxproctype_overlast] ;
	int		maxusers ;
	int		maxents ;
	vint		waiters ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	aflag		fcapture ;
	utmpacc() noex {
	    init	(this,utmpaccmem_init) ;
	    fini	(this,utmpaccmem_fini) ;
	    capbegin	(this,utmpaccmem_capbegin) ;
	    capend	(this,utmpaccmem_capend) ;
	    begin	(this,utmpaccmem_begin) ;
	    end		(this,utmpaccmem_end) ;
	    runlevel	(this,utmpaccmem_runlevel) ;
	    users	(this,utmpaccmem_users) ;
	} ; /* end ctor */
	int boottime	(time_t *) noex ;
	int irunlevel	() noex ;
	int iusers	(int) noex ;
	int entsid	(ARG *,pid_t) noex ;
	int entstat	(ARG *,pid_t) noex ;
	int entline	(ARG *,cchar *,int) noex ;
	int stats	(utmpacc_sb *) noex ;
	int extract	(int) noex ;
	int scan	(time_t) noex ;
	int getentsid	(ARG *,pid_t) noex ;
	int getentstat	(ARG *,pid_t) noex ;
	int getentline	(ARG *,cchar *,int) noex ;
	int getextract	(int) noex ;
    private:
	int iinit	() noex ;
	int ifini	() noex ;
	int icapbegin	(int) noex ;
	int icapend	() noex ;
	int ibegin	() noex ;
	int iend	() noex ;
	int load_runlevel(custime,con utmpx *) noex ;
	int load_boottime(custime,con utmpx *) noex ;
    } ; /* end struct (utmpacc) */
    struct vars {
	int		entbuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	utmpacc_atforkbefore() noex ;
    local void	utmpacc_atforkafter() noex ;
    local void	utmpacc_exit() noex ;
}

local bool isourtype(utmpx *up) noex {
	bool	f = false ;
	f = f || (up->ut_type == UTMPACC_TPROCINIT) ;
	f = f || (up->ut_type == UTMPACC_TPROCUSER) ;
	f = f || (up->ut_type == UTMPACC_TPROCLOGIN) ;
	return f ;
} /* end subroutine (isourtype) */

local int	utmpx_eterm(CUTMPX *) noex ;
local int	utmpaccent_loada(ARG *,CUTMPX *) noex ;

[[maybe_unused]] local int	debflags	(int) noex ;
[[maybe_unused]] local int	debdesc		(int) noex ;
[[maybe_unused]] local int	debsize		(int) noex ;


/* local variables */

static vars		var ;
static utmpacc		utmpacc_data ;
cbool			f_debug		= CF_DEBUG ;


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

int utmpacc_getstats(utmpacc_sb *usp) noex {
	return utmpacc_data.stats(usp) ;
}

int utmpacc_extract(int fd) noex {
	return utmpacc_data.extract(fd) ;
} /* end subroutine (utmpacc_extract) */

namespace {
    class utmpacc_icur {
	static constexpr uint	magicvalue = UTMPACC_CURMAGIC ;
	filer		*fbp ;
	uint		magval ;
	int entbegin	() noex ;
	int entend	() noex ;
	int bufbegin	() noex ;
	int bufend	() noex ;
	int openbegin	() noex ;
	int openend	() noex ;
    public:
	utmpx		*entp ;
	int		fd ;
	int start	() noex ;
	int finish	() noex ;
	int extract	() noex ;
	int entenum	(utmpx **) noex ;
	utmpacc_icur() noex : fbp(nullptr), entp(nullptr), fd(-1) { 
	    magval = 0 ;
	} ; /* end ctor */
	void dtor() noex ;
	destruct utmpacc_icur() {
	    if (magval) {
		dtor() ;
	    }
	} ; /* end destruct */
    } ; /* end struct (utmpacc_icur) */
} /* end namespace */

typedef utmpacc_icur	icur ;

local int icur_ctor(icur *icp) noex {
    	int		rs = SR_FAULT ;
	if (icp) { /* C++ UB otherwise (if done directly) */
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (icur_ctor) */

local int icur_dtor(icur *icp) noex {
    	int		rs = SR_FAULT ;
	if (icp) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (icur_dtor) */

int icur::start() noex {
    	int		rs ;
	if ((rs = icur_ctor(this)) >= 0) ylikely {
	    if ((rs = entbegin()) >= 0) ylikely {
		if ((rs = openbegin()) >= 0) ylikely {
		    if ((rs = bufbegin()) >= 0) ylikely {
			if ((rs = extract()) >= 0) ylikely {
			    magval = magicvalue ;
			} /* end if (extract) */
		        if (rs < 0) {
			    bufend() ;
		        } /* end if (error) */
		    } /* end if (icur_bufbegin) */
		    if (rs < 0) {
			openend() ;
		    } /* end if (error) */
		} /* end if (icur_openbegin) */
		if (rs < 0) {
		    entend() ;
		} /* end if (error) */
	    } /* end if (icur_startent) */
	    if (rs < 0) {
		icur_dtor(this) ;
	    } /* end if (error) */
	} /* end if (icur_ctor) */
	return rs ;
} /* end method (icur::start) */

int icur::finish() noex {
    	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (magval == magicvalue) ylikely {
	    rs = SR_OK ;
	    if (fbp) {
	        rs1 = bufend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (fd >= 0) {
	        rs1 = openend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    if (entp) {
	        rs1 = entend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    magval = 0 ;
	    {
		rs1 = icur_dtor(this) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end method (icur::finish) */

int icur::openbegin() noex {
	cint		of = (O_CLOEXEC | O_MINFD) ;
	cmode		om = S_IFREG | 0664 ;
    	int		rs ;
	if ((rs = opentmp(nullptr,of,om)) >= 0) {
	    fd = rs ;
	}
	return rs ;
} /* end method (icur::openvegin) */

int icur::openend() noex {
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (fd >= 0) {
	    rs = SR_OK ;
	    {
	        rs1 = uc_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	        fd = -1 ;
	    }
	} /* end if (open) */
	return rs ;
} /* end method (icur::openend) */

int icur::bufbegin() noex {
    	int		rs = SR_NOTOPEN ;
	if (fd >= 0) {
	    cint	fsz = szof(filer) ;
	    if (void *vp ; (rs = libmem.call(1,fsz,&vp)) >= 0) ylikely {
	        rs = SR_BUGCHECK ;
	        if ((fbp = new(vp) filer) != nullptr) {
		    rs = fbp->start(fd,0z,0,0) ;
		    if (rs < 0) {
		        destroy_at(fbp) ;
		        fbp = nullptr ;
		    } /* end if (error) */
	        } /* end if (filer_start) */
	        if (rs < 0) {
		    libmem.free(vp) ;
	        } /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (open) */
	return rs ;
} /* end method (icur::bufbegin) */

int icur::bufend() noex {
    	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if (fbp) ylikely {
	    rs = SR_OK ;
	    {
		rs1 = fbp->finish ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		destroy_at(fbp) ;
	    }
	    {
		rs1 = libmem.free(fbp) ;
		if (rs >= 0) rs = rs1 ;
		fbp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (valid) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (icur::bufend) */

int icur::entbegin() noex {
    	cint		esz = szof(utmpx) ;
    	int		rs ;
	if (void *vp ; (rs = libmem.call(1,esz,&vp)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
	    if ((entp = new(vp) utmpx) != nullptr) ylikely {
		rs = SR_OK ;
	    }
	    if (rs < 0) {
		libmem.free(vp) ;
	    } /* end if (error) */
	} /* end if (memory-acquire) */
	return rs ;
} /* end method (icur::entbegin) */

int icur::entend() noex {
    	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if (entp) ylikely {
	    rs = SR_OK ;
	    {
		destroy_at(entp) ;
	    }
	    {
		rs1 = libmem.free(entp) ;
		if (rs >= 0) rs = rs1 ;
		entp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (valid) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (icur::entend) */

int icur::extract() noex {
    	int		rs = SR_NOTOPEN ;
	if (fd >= 0) {
    	    rs = fd ;
	} /* end if (open) */
    	return rs ;
} /* end method (icur::extract) */

int icur::entenum(utmpx **epp) noex {
    	int		rs = SR_BUGCHECK ;
	int		len = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (fbp && epp) ylikely {
    	    cint esz = szof(utmpx) ;
	    DPRINTF("esz=%d\n",esz) ;
	    *epp = nullptr ;
    	    if ((rs = fbp->read(entp,esz,-1)) > 0) {
	        DPRINTF("fb.read() rs=%d\n",rs) ;
		*epp = entp ;
		len = rs ;
	    } /* end if (filer_read) */
	} /* end if (valid) */
	DPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end method (icur::entenum) */

void icur::dtor() noex {
	if (cint rs = finish() ; rs < 0) {
	    ulogerror("utmpacc",rs,"icur-destruct") ;
	} /* end if */
} /* end method (icur::dtor) */

int utmpacc_curbegin(utmpacc_cur *curp) noex {
	int		rs = SR_FAULT ;
	DPRINTF("ent\n") ;
	if (curp) ylikely {
	    cint	csz = szof(utmpacc_icur) ;
	    curp->icursorp = nullptr ;
	    DPRINTF("-> mail csz=%d\n",csz) ;
	    if (void *vp ; (rs = libmem.mall(csz,&vp)) >= 0) ylikely {
		rs = SR_BUGCHECK ;
		if (icur *icurp = new(vp) icur ; icurp) ylikely {
		    if ((rs = icurp->start()) >= 0) ylikely {
			cint fd = icurp->fd ;
			if ((rs = utmpacc_extract(fd)) >= 0) ylikely {
			    if ((rs = uc_rewind(fd)) >= 0) ylikely {
	    			curp->icursorp = icurp ;
				if_constexpr (f_debug) {
				    debsize(fd) ;
				}
			    } /* end if (uc_rewind) */
			} /* end if (utmpacc_extract) */
			if (rs < 0) {
			    icurp->finish() ;
			} /* end if (error) */
		    } /* end if (icur_start) */
		    if (rs < 0) {
			destroy_at(icurp) ;
			icurp = nullptr ;
		    } /* end if (error) */
		} /* end if (new-icur) */
		if (rs < 0) {
		    libmem.free(vp) ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (utmpacc_curbegin) */

int utmpacc_curenum(utmpacc_cur *curp,utmpacc_ent *ep,char *eb,int el) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (curp && ep && eb) ylikely {
	    utmpacc_icur *icurp = resumelife<utmpacc_icur>(curp->icursorp) ;
	    rs = SR_NOTOPEN ;
	    memclear(ep) ;
	    memclear(eb,el) ;
	    if (icurp) ylikely {
		if (utmpx *uep ; (rs = icurp->entenum(&uep)) > 0) ylikely {
		    DPRINTF("ent.type=%hu\n",uep->ut_type) ;
		    DPRINTF("ent.user=%s\n",uep->ut_user) ;
		    rs = utmpaccent_load(ep,eb,el,uep) ;
		    len = rs ;
		} /* end if (icur_entenum) */
	    } /* end if (internal-cursor-pointer ok) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (utmpacc_curenum) */

int utmpacc_curend(utmpacc_cur *curp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if (curp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->icursorp) ylikely {
	        utmpacc_icur *icurp = resumelife<utmpacc_icur>(curp->icursorp) ;
		rs = SR_OK ;
		{
		    DPRINTF("icur-fin\n") ;
		    rs1 = icurp->finish() ;
		    if (rs >= 0) rs = rs1 ;
		    DPRINTF("icur-fin rs=%d\n",rs) ;
		} /* end if (memory-release) */
		{
		    destroy_at(icurp) ;
		}
		{
		    DPRINTF("icur-free\n") ;
		    rs1 = libmem.free(curp->icursorp) ;
		    if (rs >= 0) rs = rs1 ;
		    curp->icursorp = nullptr ;
		    DPRINTF("icur-free rs=%d\n",rs) ;
		} /* end if (memory-release) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (utmpacc_curend) */


/* local subroutines */

int utmpacc_co::callout(int v) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
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
} /* end method (utmpacc_co::callout) */

int utmpacc::iinit() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = omx.create) >= 0) ylikely {
	            if ((rs = ocv.create) >= 0) ylikely {
	                void_f	b = utmpacc_atforkbefore ;
	                void_f	a = utmpacc_atforkafter ;
	                if ((rs = uc_atforkrec(b,a,a)) >= 0) {
	                    if ((rs = uc_atexit(utmpacc_exit)) >= 0) {
	                        f = true ;
	                        finitdone = true ;
	                    } /* end if (uc_atexit) */
	                    if (rs < 0) {
	                        uc_atforkexp(b,a,a) ;
			    } /* end if (error) */
	                } /* end if (uc_atfork) */
	                if (rs < 0) {
	                    ocv.destroy() ;
			} /* end if (error) */
	            } /* end if (ptc_create) */
	            if (rs < 0) {
	                omx.destroy() ;
		    } /* end if (error) */
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		} /* end if (error) */
	    } else if (!finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rsl = SR_OK ;
	            if (!finit) {
		        rsl = SR_LOCKFAIL ;
	            } else if (finitdone) {
		        rsl = 1 ;
	            }
	            return rsl ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;			/* <- time-watching */
	    } /* end if (initialization) */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
} /* end method (utmpacc::iinit) */

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
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ocv.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = omx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
} /* end method (utmpacc::ifini) */

int utmpacc::icapbegin(int to) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = omx.lockbegin(to)) >= 0) ylikely {
	    waiters += 1 ;
	    while ((rs >= 0) && fcapture) { /* busy */
	        rs = ocv.wait(&omx,to) ;
	    } /* end while */
	    if (rs >= 0) {
	        fcapture = true ;
	    }
	    waiters -= 1 ;
	    rs1 = omx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end method (utmpacc::icapbegin) */

int utmpacc::icapend() noex {
	int		rs ;
	int		rs1 ;
	if ((rs = omx.lockbegin) >= 0) ylikely {
	    fcapture = false ;
	    if (waiters > 0) {
	        rs = ocv.signal ;
	    }
	    rs1 = omx.lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
} /* end method (utmpacc::icapend) */

int utmpacc::ibegin() noex {
	return SR_OK ;
} /* end method (utmpacc::begin) */

int utmpacc::iend() noex {
	return SR_OK ;
} /* end method (utmpacc::end) */

int utmpacc::boottime(time_t *rp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rp) ylikely {
	    *rp = 0 ;
	    if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
	            if ((rs = capbegin) >= 0) ylikely {
		        if ((rs = begin) >= 0) ylikely {
	                    custime	dt = time(nullptr) ;
	                    cint	to = UTMPACC_INTBOOT ;
	                    if ((dt - btime.t) >= to) {
	                        rs = scan(dt) ;
	                    } /* end if (timed-out) */
		        } /* end if */
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
	            if (rs >= 0) {
			*rp = btime.v ;
		    }
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblocker) */
	} /* end if (non-null) */
	return rs ;
} /* end method (utmpacc::boottime) */

int utmpacc::irunlevel() noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	    if ((rs = init) >= 0) ylikely {
	        if ((rs = capbegin) >= 0) ylikely {
		    if ((rs = begin) >= 0) ylikely {
	                custime		dt = time(nullptr) ;
	                cint		to = UTMPACC_INTRUNLEVEL ;
	                if ((dt - rlevel.t) >= to) {
	                    rs = scan(dt) ;
	                } /* end if */
	                if (rs >= 0) {
			    n = (rlevel.v) ? rlevel.v : mkchar('°') ;
			}
		    } /* end if */
	            rs1 = capend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (capture-exclusion) */
	    } /* end if (init) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (sigblocker) */
	return (rs >= 0) ? n : rs ;
} /* end method (utmpacc::irunlevel) */

int utmpacc::iusers(int w) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	int		n = 0 ;
	if ((w >= 0) && (w < UTMPACC_NTYPES)) ylikely {
	    if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
	            if ((rs = capbegin) >= 0) ylikely {
		        if ((rs = begin) >= 0) ylikely {
	                    custime	dt = time(nullptr) ;
	                    cint	to = UTMPACC_INTUSERS ;
	                    if ((dt - nusers[w].t) >= to) {
	                        rs = scan(dt) ;
	                    } /* end if */
	                    if (rs >= 0) {
				n = nusers[w].v ;
			    }
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
} /* end method (utmpacc::iusers) */

int utmpacc::entsid(ARG *ap,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ffound = false ;
	if (ap && ap->uep && ap->uebuf) ylikely {
	    memclear(ap->uep) ;
	    ap->uebuf[0] = '\0' ;
	    if (sid <= 0) sid = getsid(0) ;
	    if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
	            if ((rs = capbegin) >= 0) ylikely {
		        if ((rs = begin) >= 0) ylikely {	
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
} /* end method (utmpacc::entsid) */

int utmpacc::entstat(ARG *ap,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ffound = false ;
	if (ap && ap->uep && ap->uebuf) ylikely {
	    memclear(ap->uep) ;
	    ap->uebuf[0] = '\0' ;
	    if (sid <= 0) sid = getsid(0) ;
	    if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
	            if ((rs = capbegin) >= 0) ylikely {
		        if ((rs = begin) >= 0) ylikely {	
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
	    if ((rs >= 0) && (ap->uep->line == nullptr)) {
		rs = SR_NOTFOUND ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ffound : rs ;
} /* end method (utmpacc::entstat) */

int utmpacc::entline(ARG *ap,cchar *lp,int ll) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ffound = false ;
	if (ap && ap->uep && ap->uebuf && lp) ylikely {
	    memclear(ap->uep) ;
	    ap->uebuf[0] = '\0' ;
	    if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
	            if ((rs = capbegin) >= 0) ylikely {
		        if ((rs = begin) >= 0) ylikely {	
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
	    if ((rs >= 0) && (ap->uep->line == nullptr)) {
		rs = SR_NOTFOUND ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ffound : rs ;
} /* end method (utmpacc::entline) */

int utmpacc::stats(utmpacc_sb *usp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		mu = 0 ;
	if (usp) ylikely {
	    memclear(usp) ;
	    if (sigblocker b ; (rs = b.start) >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
	            if ((rs = capbegin) >= 0) ylikely {
		        if ((rs = begin) >= 0) ylikely {
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
} /* end method (utmpacc::stats) */

int utmpacc::extract(int fd) noex {
	int		rs = SR_BADF ;
	int		rs1 ;
	int		wlen = 0 ;
	DPRINTF("ent fd=%d\n",fd) ;
	if (fd >= 0) ylikely {
	    if (sigblocker b ; (rs = b.start) >= 0) ylikely {
		DPRINTF("-> init\n") ;
	        if ((rs = init) >= 0) ylikely {
		    DPRINTF("-> cap\n") ;
	            if ((rs = capbegin) >= 0) ylikely {
		        DPRINTF("-> begin\n") ;
		        if ((rs = begin) >= 0) ylikely {
			    {
		        	DPRINTF("-> getextract\n") ;
			        rs = getextract(fd) ;
			        wlen = rs ;
		        	DPRINTF(" getextract() rs=%d\n",rs) ;
			    } /* end block */
			    rs1 = u_rewind(fd) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (begin) */
		        DPRINTF("begin-out rs=%d\n",rs) ;
	                rs1 = capend ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (capture-exclusion) */
		    DPRINTF("cap-out rs=%d\n",rs) ;
	        } /* end if (init) */
		DPRINTF("init-out rs=%d\n",rs) ;
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (valid) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (utmpacc::extract) */

int utmpacc::scan(time_t dt) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		nu = 0 ; /* calculated */
	int		ne = 0 ; /* calculated */
	memclear(nusers,(szof(utmpacc_item) * utxproctype_overlast)) ;
	setutxent() ;
	for (CUTMPX *up ; (up = getutxent()) != np ; ) {
	    int		pt = -1 ;
	    switch (up->ut_type) {
	    case UTMPACC_TRUNLEVEL:
		rs = load_runlevel(dt,up) ;
		break ;
	    case UTMPACC_TBOOTTIME:
		rs = load_boottime(dt,up) ;
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
	    if (rs < 0) break ;
	} /* end for */
	if (rs >= 0) {
	    for (int i = 0 ; i < utxproctype_overlast ; i += 1) {
	        nusers[i].t = dt ;
	    } /* end for */
	    if (nu > maxusers) maxusers = nu ;
	    if (ne > maxents) maxents = ne ;
	} /* end if (ok) */
	return rs ;
} /* end method (utmpacc::scan) */

int utmpacc::getentsid(ARG *ap,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		ffound = false ; /* return-value */
	if (ap) ylikely {
	    time_t	dt = 0z ;
	    rs = SR_OK ;
	    setutxent() ;
	    for (CUTMPX *up ; (up = getutxent()) != nullptr ; ) {
	        switch (up->ut_type) {
	        case UTMPACC_TRUNLEVEL:
		    if (dt == 0L) dt = time(nullptr) ;
		    rs = load_runlevel(dt,up) ;
		    break ;
	        case UTMPACC_TBOOTTIME:
		    if (dt == 0L) dt = time(nullptr) ;
		    rs = load_boottime(dt,up) ;
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
	} /* end if (non-null) */
	return (rs >= 0) ? ffound : rs ;
} /* end method (utmpacc::getentsid) */

int utmpacc::getentstat(ARG *ap,pid_t sid) noex {
    	cnullptr	np{} ;
	cint		tlen = TERMBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		ffound = false ;
	cchar		*devprefix = DEVPREFIX ;
	char		tbuf[tlen+1] ;
	if ((rs = sncpy(tbuf,tlen,devprefix)) >= 0) ylikely {
	    cint	tl = rs ;
	    setutxent() ;
	    for (utmpx *up ; (up = getutxent()) != np ; ) {
	       if (isourtype(up)) {
		    cint	ll = utmpsize.line ;
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
	    } /* end for */
	} /* end if (sncpy) */
	return (rs >= 0) ? ffound : rs ;
} /* end method (utmpacc::getentstat) */

int utmpacc::getentline(ARG *ap,cchar *lp,int ll) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		ffound = false ; /* return-value */
	if (ap) ylikely {
	    time_t	dt = 0z ;
	    setutxent() ;
	    for (CUTMPX *up ; (up = getutxent()) != np ; ) {
	        switch (up->ut_type) {
	        case UTMPACC_TRUNLEVEL:
		    if (dt == 0L) dt = time(nullptr) ;
		    rs = load_runlevel(dt,up) ;
		    break ;
	        case UTMPACC_TBOOTTIME:
		    if (dt == 0L) dt = time(nullptr) ;
		    rs = load_boottime(dt,up) ;
		    break ;
	        case UTMPACC_TPROCINIT:
	        case UTMPACC_TPROCLOGIN:
	        case UTMPACC_TPROCUSER:
	            if (up->ut_user[0] != '\0') {
		        if (ap->uep->line == nullptr) {
			    cint sline = utmpsize.line ;
	                    if (strnncmp(up->ut_line,sline,lp,ll) == 0)  {
			        ffound = true ;
			        rs = utmpaccent_loada(ap,up) ;
		            } /* end if (matched) */
	                } /* end if (line-field was empty) */
	            } /* end if (non-nul) */
		    break ;
	        } /* end switch (UTMP entry types) */
	        if (ffound) break ;
	        if (rs < 0) break ;
	    } /* end for (reading UTMPX entries) */
	    endutxent() ;
	} /* end if (non-null) */
	return (rs >= 0) ? ffound : rs ;
} /* end method (utmpacc::getentline) */

int utmpacc::getextract(int fd) noex {
    	cnullptr	np{} ;
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent fd=%d\n",fd) ;
	if (fd >= 0) {
	    if (filer fb ; (rs = fb.start(fd,0z,0,0)) >= 0) {
	        cint usz = szof(utmpx) ;
		DPRINTF("usz=%d\n",usz) ;
	        {
	            setutxent() ;
	            for (CUTMPX *up ; (up = getutxent()) != np ; ) {
		        DPRINTF("ent->type=%hu\n",up->ut_type) ;
		        DPRINTF("ent->user=%s\n",up->ut_user) ;
		        rs = fb.write(up,usz) ;
		        DPRINTF("fb.write() rs=%d\n",rs) ;
		        wlen += rs ;
	                if (rs < 0) break ;
	            } /* end for (reading UTMPX entries) */
	            endutxent() ;
	        } /* end block */
	        DPRINTF("block-out rs=%d\n",rs) ;
	        rs1 = fb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    if_constexpr (f_debug) {
		if (rs >= 0) {
		    debsize(fd) ;
		}
	    } /* end if_constexor (f_debug) */
	} /* end if (valid) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (utmpacc::getextract) */

int utmpacc::load_runlevel(custime dt,con utmpx *up) noex {
    	int		rs ;
	if ((rs = utmpx_eterm(up)) >= 0) ylikely {
	    cint eterm = (rs > 0) ? rs : mkchar('×') ;
	    rlevel.v = mkchar(eterm) ;
	    rlevel.t = dt ;
	} /* end if (utmpx_eterm) */
	return rs ;
} /* end method (utmpacc::load_runlevel) */

int utmpacc::load_boottime(custime dt,con utmpx *up) noex {
	btime.v = uint(up->ut_tv.tv_sec) ;
	btime.t = dt ;
	return SR_OK ;
} /* end method (utmpacc::load_boottime) */

local void utmpacc_atforkbefore() noex {
	utmpacc_data.capbegin() ;
} /* end subroutine (utmpacc_atforkbefore) */

local void utmpacc_atforkafter() noex {
	utmpacc_data.capend() ;
} /* end subroutine (utmpacc_atforkafter) */

local void utmpacc_exit() noex {
	if (cint rs = utmpacc_fini() ; rs < 0) {
	    ulogerror("utmpacc",rs,"exit-fini") ;
	}
} /* end subroutine (utmpacc_exit) */

local int utmpaccent_loada(ARG *ap,CUTMPX *up) noex {
	return utmpaccent_load(ap->uep,ap->uebuf,ap->uelen,up) ;
} /* end subroutine */

/* special local subroutines (depending on OS capability) */

#if	defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0)

local int utmpx_eterm(CUTMPX *up) noex {
	return up->ut_exit.e_termination ;
} /* end subroutine */

#else /* defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0) */

local int utmpx_eterm(CUTMPX *) noex {
	return SR_OK ;
} /* end subroutine */

#endif /* defined(SYSHASUTMP_EXIT) && (SYSHASUTMP_EXIT > 0) */

utmpacc_enter::operator int () noex {
    	int		rs ;
    	if (static cint	rsv = var ; (rs = rsv) >= 0) ylikely {
	    rs = var.entbuflen ;
	}
    	return rs ;
} /* end method (utmpacc_enter::operator) */

vars::operator int () noex {
	entbuflen = (szof(utmpx) + 4) ; /* <- four (4) NUL terminators */
	return SR_OK ;
} /* end method (vars::operator) */

local int debflags(int of) noex {
    	int		rs ;
	int		rs1 ;
	if (char *lbuf ; (rs = libmem.ml(&lbuf)) >= 0) ylikely {
	    if ((rs = snflagsopen(lbuf,rs,of)) >= 0) ylikely {
		DPRINTF("ofl %s\n",lbuf) ;
	    } /* end if (snflagsopen) */
	    rs1 = libmem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-d) */
	return rs ;
} /* end subroutine (debflags) */

local int debdesc(int fd) noex {
    	int		rs ;
	if ((rs = u_fgetfl(fd)) >= 0) ylikely {
	    debflags(rs) ;
	} /* end if (u_fgetfl) */
	return rs ;
} /* end subroutine (debdesc) */

local int debsize(int fd) noex {
    	int		rs ;
	if ((rs = u_fsize(fd)) >= 0) ylikely {
	    DPRINTF("fsz=%d\n",rs) ;
	} /* end if (u_fsize) */
	return rs ;
} /* end subroutine (debsize) */


