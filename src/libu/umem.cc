/* umem SUPPORT (3uc) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	um

	Description:
	This is the low-level component of the MEMALLOC facility.
	These subroutines need to be able to be interposed upon,
	so they have to be in their own compilation (object) image.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdckdint>		/* |ckd_mu(3c++)| */
#include	<cstring>		/* |strncpy(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<utimeout.h>
#include	<errtimer.hh>
#include	<sysconfcmds.h>
#include	<localmisc.h>

#include	"umem.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |getlenstr(3u)| + |memclear(3u)| */
import ulibvals ;			/* |getlenstr(3u)| + |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct valcmds {
	inline static cint	maxargs		= _SC_ARG_MAX ;
	inline static cint	maxline		= _SC_LINE_MAX ;
	inline static cint	maxname		= _SC_NAME_MAX ;
	inline static cint	maxpath		= _SC_PATH_MAX ;
	inline static cint	maxnode		= _SC_NODENAME_MAX ;
    } ; /* end struct (valcmds) */
} /* end namespace */

namespace {
    struct valmgr ;
    struct valmgr_vals {
	int	maxargslen ;
	int	maxlinelen ;
	int	maxnamelen ;
	int	maxpathlen ;
	int	maxnodelen ;
    } ; /* end struct (valmgr_vals) */
    struct valmgr {
	valmgr_vals	v ;
	int operator () (int) noex ;
	int get(int) noex ;
    } ; /* end struct (valmgr) */
} /* end namespace */

namespace {
    struct umgr ;
    typedef int (umgr::*umgr_m)(int,void *) noex ;
    struct umgr {
	umgr_m		m ;
	void		*cp ;		/* constant-void-pointer */
	umgr(void *op = nullptr) noex : cp(op) { } ;
	int operator () (int,void *) noex ;
	sysret_t stdmalloc	(int,void *) noex ;
	sysret_t stdvalloc	(int,void *) noex ;
	sysret_t stdrealloc	(int,void *) noex ;
	sysret_t stdfree	(int,void *) noex ;
    } ; /* end struct (umgr) */
} /* end namespace */


/* forward references */


/* local vaiables */

constexpr valcmds	valcmd ;

static valmgr		valer ;

static cint		pagesz		= ulibval.pagesz ;


/* exported variables */

namespace libu {
    umems	um ;
    umems	umem ;
} /* end namespace (libu) */


/* exported subroutines */

namespace libu {
    int umems::strw(cchar *sp,int µsl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		bl = 0 ; /* return-value */
	if (sp && rpp) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	        if (char *bp ; (rs = mall((sl + 1),&bp)) >= 0) ylikely {
		    bl = sl ;
	            *rpp = bp ;
	            strncpy(bp,sp,sl) ;
	            bp[bl] = '\0' ;
	        } else {
		    *rpp = nullptr ;
	        } /* end if (mall) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? bl : rs ;
    } /* end method (umems::strw) */
    int umems::mall(int sz,void *vpp) noex {
	umgr	lmo ;
	lmo.m = &umgr::stdmalloc ;
	return lmo(sz,vpp) ;
    } /* end subroutine (umems::mall) */
    int umems::vall(int sz,void *vpp) noex {
	umgr	lmo ;
	lmo.m = &umgr::stdvalloc ;
	return lmo(sz,vpp) ;
    } /* end subroutine (umems::vall) */
    int umems::call(int ne,int esz,void *vpp) noex {
	int		rs = SR_TOOBIG ;
	int		sz = 0 ; /* return-value */
	if (ckd_mul(&sz,ne,esz) == false) {
	    if ((rs = mall(sz,vpp)) >= 0) ylikely {
		caddr_t	*epp = caddrp(vpp) ;
		rs = SR_BUGCHECK ;
		if (caddr_t ca = caddr_t(*epp) ; ca) {
	            rs = memclear(ca,sz) ;
		}
	    } /* end if (umems::mall) */
	} /* end if (valid) */
	return (rs >= 0) ? sz : rs ;
    } /* end subroutine (umems::call) */
    int umems::rall(void *cp,int sz,void *vpp) noex {
	int		rs = SR_FAULT ;
	if (cp) ylikely {
	    const uintptr_t	am = (szof(uintptr_t) - 1) ;
	    const uintptr_t	a = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if (a && ((a & am) == 0)) ylikely {
	        umgr	lmo(cp) ;
	        lmo.m = &umgr::stdrealloc ;
	        rs = lmo(sz,vpp) ;
	    } /* end if (aligned correctly) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (umems::rall) */
    int umems::free(void *cp) noex {
	int		rs = SR_FAULT ;
	if (cp) ylikely {
	    const uintptr_t	am = (szof(uintptr_t) - 1) ;
	    const uintptr_t	a = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if (a && ((a & am) == 0)) ylikely {
	        umgr	lmo(cp) ;
	        lmo.m = &umgr::stdfree ;
	        rs = lmo(1,cp) ;
	    } /* end if (aligned correctly) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (umems::free) */
    int umems::rsfree(int rs,void *p) noex {
	if (p) ylikely {
    	    if (cint rs1 = free(p) ; rs >= 0) ylikely {
		rs = rs1 ;
	    }
	} else {
    	    if (rs >= 0) rs = SR_FAULT ;
	}
	return rs ;
    } /* end subroutine (umems::rsfree) */
} /* end namespace (libu) */

namespace libu {
    int umems::ps(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rpp) ylikely {
	    if ((rs = pagesz) >= 0) {
	        len = rs ;
	        rs = vall(len,rpp) ;
	    }
	}
	return (rs >= 0) ? len : rs ;
    } /* end method (umems::ps) */
} /* end namespace (libu) */

namespace libu {
    local int bufx(cint cmd,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rpp) ylikely {
	    if ((rs = valer(cmd)) >= 0) {
	        len = rs ;
	        rs = umem.mall((len + 1),rpp) ;
	    }
	}
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (bufx) */
} /* end namespace (libu) */

namespace libu {
    int umems::ma(cchar **rpp) noex {
	return bufx(valcmd.maxargs,rpp) ;
    } /* end method (umems::ma) */
    int umems::ml(cchar **rpp) noex {
	return bufx(valcmd.maxline,rpp) ;
    } /* end method (umems::ml) */
    int umems::mn(cchar **rpp) noex {
	return bufx(valcmd.maxname,rpp) ;
    } /* end method (umems::mn) */
    int umems::mp(cchar **rpp) noex {
	return bufx(valcmd.maxname,rpp) ;
    } /* end method (umems::mp) */
    int umems::nn(cchar **rpp) noex {
	return bufx(valcmd.maxnode,rpp) ;
    } /* end method (umems::nn) */
} /* end namespace (libu) */


/* local subroutines */

int umgr::operator () (int sz,void *vp) noex {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	int     	rs = SR_FAULT ;
	if (vp) ylikely {
	    rs = SR_INVALID ;
	    if (sz > 0) ylikely {
	        reterr	r ;
	        repeat {
	            if ((rs = (this->*m)(sz,vp)) < 0) {
		        r(rs) ;			/* <- default causes exit */
                        switch (rs) {
                        case SR_AGAIN:
                            r = to_again(rs) ;
                            break ;
                        case SR_BUSY:
                            r = to_busy(rs) ;
                            break ;
                        case SR_NOMEM:
                            r = to_nomem(rs) ;
                            break ;
	                case SR_INTR:
		            r(false) ;
	                    break ;
			} /* end switch */
			rs = r ;
	            } /* end if (std-call) */
	        } until ((rs >= 0) || r.fexit) ;
	    } /* end if (valid size) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (umgr::operator) */

sysret_t umgr::stdmalloc(int sz,void *vp) noex {
    	cnullptr	np{} ;
	csize		msize = size_t(sz) ;
	int		rs ;
	void		**rpp = voidpp(vp) ;
	errno = 0 ;
	if (void *rp ; (rp = malloc(msize)) != np) ylikely {
	    rs = sz ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
} /* end method (umgr::stdmalloc) */

sysret_t umgr::stdvalloc(int sz,void *vp) noex {
    	cnullptr	np{} ;
	csize		msize = size_t(sz) ;
	int		rs ;
	void		**rpp = voidpp(vp) ;
	errno = 0 ;
	if (void *rp ; (rp = valloc(msize)) != np) ylikely {
	    rs = sz ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
} /* end method (umgr::stdvalloc) */

sysret_t umgr::stdrealloc(int sz,void *vp) noex {
    	cnullptr	np{} ;
	csize		msize = size_t(sz) ;
	void		**rpp = voidpp(vp) ;
	int		rs ;
	errno = 0 ;
	if (void *rp ; (rp = realloc(cp,msize)) != np) ylikely {
	    rs = sz ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
} /* end method (umgr::stdrealloc) */

sysret_t umgr::stdfree(int,void *) noex {
    	int		rs = SR_OK ;
	errno = 0 ;
	free(cp) ;
	if (errno) {
	    rs = (- errno) ;
	}
	return rs ;
} /* end method (umgr::stdfree) */

#define	RETVAL(name,cmd) 			\
	({					\
	    int rsl ;				\
	    if ((rsl = name) == 0) {		\
		rsl = get(cmd) ;		\
	        name = rsl ;			\
	    } ; rsl ;				\
	})

int valmgr::operator () (int cmd) noex {
    	int		rs = SR_BUGCHECK;
	switch (cmd) {
	case valcmd.maxargs:
	    rs = RETVAL(v.maxargslen,cmd) ;
	    break ;
	case valcmd.maxline:
	    rs = RETVAL(v.maxlinelen,cmd) ;
	    break ;
	case valcmd.maxname:
	    rs = RETVAL(v.maxnamelen,cmd) ;
	    break ;
	case valcmd.maxpath:
	    rs = RETVAL(v.maxpathlen,cmd) ;
	    break ;
	case valcmd.maxnode:
	    rs = RETVAL(v.maxnodelen,cmd) ;
	    break ;
	} /* end switch */
	return rs ;
} /* end method (valmgr::operator) */

int valmgr::get(int cmd) noex {
	return u_sysconfval(cmd,nullptr) ;
} /* end method (valmgr::get) */


