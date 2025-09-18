/* uclibmem SUPPORT (3uc) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A�D� Morano
	This code was originally written.

	= 2018-09-26, David A-D- Morano
	I brought |uc_libmem.mall(3uc)| in line w/ the standard for
	|calloc(3c)|.  I never used this myself in 40 years, so I
	never missed it!

*/

/* Copyright � 1998,2018 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_libmemallocstrw
	uc_libmemallocitem
	uc_libmemalloc
	uc_libmemvalloc
	uc_libmemcalloc
	uc_libmemrealloc
	uc_libmemfree
	uc_libmemrsfree

	Synopsis:
	int uc_libmemallocstrw(cchar *,int,cchar **) noex ;
	int uc_libmemallocitem(cvoid *,int,voidpp) noex ;
	int uc_libmemalloc(int,void *) noex ;
	int uc_libmemvalloc(int,void *) noex ;
	int uc_libmemcalloc(int,int,void *) noex ;
	int uc_libmemrealloc(void *,int,void *) noex ;
	int uc_libmemfree(void *) noex ;
	int uc_libmemrsfree(int,void *) noex ;

	Description:
	This is the low-level component of the MEMALLOC facility.
	These subroutines need to be able to be interposed upon,
	so they have to be in their own compilation (object) image.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |stpncpy(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"uclibmem.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mgr ;
    typedef int (mgr::*mgr_m)(int,void *) noex ;
    struct mgr {
	mgr_m		m ;
	void		*cp ;		/* constant-void-pointer */
	mgr(void *op = nullptr) noex : cp(op) { } ;
	int operator () (int,void *) noex ;
	int stdmalloc(int,void *) noex ;
	int stdvalloc(int,void *) noex ;
	int stdrealloc(int,void *) noex ;
	int stdfree(int,void *) noex ;
    } ; /* end struct (mgr) */
} /* end namespace */


/* forward references */


/* local vaiables */


/* exported variables */

namespace libuc {
    libmems		libmem ;
}

using libuc::libmem ;


/* exported subroutines */

int uc_libmemallocstrw(cchar *sp,int �sl,cchar **rpp) noex {
    	return libmem.strw(sp,�sl,rpp) ;
}

int uc_libmemallocitem(cvoid *sp,int �sl,void **rpp) noex {
    	return libmem.item(sp,�sl,rpp) ;
}

int uc_libmemalloc(int sz,void *vp) noex {
    	return libmem.mall(sz,vp) ;
}

int uc_libmemvalloc(int sz,void *vp) noex {
    	return libmem.vall(sz,vp) ;
}

int uc_libmemcalloc(int ne,int esz,void *vp) noex {
    	return libmem.call(ne,esz,vp) ;
}

int uc_libmemrealloc(void *cp,int sz,void *vp) noex {
    	return libmem.rall(cp,sz,vp) ;
}

int uc_libmemfree(void *cp) noex {
    	return libmem.free(cp) ;
}

int uc_libmemrsfree(int rs,void *p) noex {
    	return libmem.rsfree(rs,p) ;
}


/* local subroutines */

namespace libuc {
    int libmems::strw(cchar *sp,int �sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (sp && rpp) {
	    if (int sl ; (sl = getlenstr(sp,�sl)) >= 0) {
	        if (char *bp ; (rs = mall((sl + 1),&bp)) >= 0) {
	            *rpp = bp ;
		    {
	                char *ep = stpncpy(bp,sp,sl) ;
		        *ep = '\0' ;
		        rl = intconv(ep - bp) ;
		    }
	        } else {
		    *rpp = nullptr ;
	        } /* end if (alloc) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
    } /* end method (libmems::strw) */
    int libmems::item(cvoid *sp,int sl,void **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (sp && rpp) {
	    rs = SR_INVALID ;
	    if (sl > 0) {
	        if (char *bp ; (rs = mall((sl + 1),&bp)) >= 0) {
	            *rpp = bp ;
		    {
	                char *ep = charp(memcopy(bp,sp,sl)) ;
		        *ep = '\0' ;
		        rl = intconv(ep - bp) ;
		    }
	        } else {
		    *rpp = nullptr ;
	        } /* end if (alloc) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
    } /* end method (libmems::item) */
    int libmems::mall(int sz,void *vp) noex {
	mgr		lmo ;
	lmo.m = &mgr::stdmalloc ;
	return lmo(sz,vp) ;
    } /* end method (libmems::mall) */
    int libmems::call(int ne,int esz,void *vp) noex {
	cint		sz = (ne * esz) ;
	int		rs ;
	if ((rs = mall(sz,vp)) >= 0) {
	    memclear(vp,sz) ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end method (libmems::call) */
    int libmems::vall(int sz,void *vp) noex {
	mgr		lmo ;
	lmo.m = &mgr::stdvalloc ;
	return lmo(sz,vp) ;
    } /* end method (libmems::vall) */
    int libmems::rall(void *cp,int sz,void *vp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const uintptr_t	am = (szof(uintptr_t) - 1) ;
	    const uintptr_t	v = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & am) == 0) {
	        mgr	lmo(cp) ;
	        lmo.m = &mgr::stdrealloc ;
	        rs = lmo(sz,vp) ;
	    } /* end if (aligned correctly) */
	} /* end if (non-null) */
	return rs ;
    } /* end method (libmems::rall) */
    int libmems::free(void *cp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const uintptr_t	am = (szof(uintptr_t) - 1) ;
	    const uintptr_t	v = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & am) == 0) {
	        mgr	lmo(cp) ;
	        lmo.m = &mgr::stdfree ;
	        rs = lmo(1,cp) ;
	    } /* end if (valid address alignment) */
	} /* end if (non-null) */
	return rs ;
    } /* end method (libmems::free) */
    int libmems::rsfree(int rs,void *p) noex {
	if (p) {
    	    if (cint rs1 = free(p) ; rs >= 0) {
		rs = rs1 ;
	    }
	} else {
    	    if (rs >= 0) rs = SR_FAULT ;
	}
	return rs ;
    } /* end method (libmems::rsfree) */
} /* end namespace (libuc) */

namespace libuc {
    int libmems::mallocstrw(cchar *sp,int �sl,cchar **rpp) noex {
	return strw(sp,�sl,rpp) ;
    }
    int libmems::mallocitem(cvoid *sp,int �sl,void **rpp) noex {
	return item(sp,�sl,rpp) ;
    }
    int libmems::malloc(int sz,void *vp) noex {
	return vall(sz,vp) ;
    }
    int libmems::calloc(int ne,int esz,void *vp) noex {
	return call(ne,esz,vp) ;
    }
    int libmems::ralloc(void *cp,int sz,void *vp) noex {
	return rall(cp,sz,vp) ;
    }
} /* end namespace (libuc) */

int mgr::operator () (int sz,void *vp) noex {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	reterr		r ;
	int     	rs = SR_FAULT ;
	if (vp) {
	    rs = SR_INVALID ;
	    if (sz > 0) {
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
}
/* end subroutine (mgr::operator) */

int mgr::stdmalloc(int sz,void *vp) noex {
	csize		msize = size_t(sz) ;
	int		rs ;
	void		**rpp = voidpp(vp) ;
	errno = 0 ;
	if (void *rp ; (rp = malloc(msize)) != nullptr) {
	    rs = sz ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
}
/* end method (mgr::stdmalloc) */

int mgr::stdvalloc(int sz,void *vp) noex {
	csize		msize = size_t(sz) ;
	int		rs ;
	void		**rpp = voidpp(vp) ;
	errno = 0 ;
	if (void *rp ; (rp = valloc(msize)) != nullptr) {
	    rs = sz ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
}
/* end method (mgr::stdvalloc) */

int mgr::stdrealloc(int sz,void *vp) noex {
	csize		msize = size_t(sz) ;
	void		*fvp = cast_const<voidp>(cp) ;
	void		**rpp = voidpp(vp) ;
	int		rs ;
	errno = 0 ;
	if (void *rp ; (rp = realloc(fvp,msize)) != nullptr) {
	    rs = sz ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
} /* end method (mgr::stdrealloc) */

int mgr::stdfree(int,void *vp) noex {
    	free(vp) ;
    	return SR_OK ;
} /* end method (mgr::stdfree) */


