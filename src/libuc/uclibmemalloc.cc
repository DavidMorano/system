/* uclibmemalloc SUPPORT (3uc) */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* memory allocation facility (for library use) */


/* revision history:

	= 1998-03-85, David A�D� Morano
	This code was originally written.

	= 2018-09-26, David A-D- Morano
	I brought |uc_libcalloc(3uc)| in line w/ the standard for
	|calloc(3c)|.  I never used this myself in 40 years, so I
	never missed it!

*/

/* Copyright � 1998,2018 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This is the low-level component of the MEMALLOC facility.
	These subroutines need to be able to be interposed upon,
	so they have to be in their own compilation (object) image.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"uclibmemalloc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct uclibmemalloc ;
    typedef int (uclibmemalloc::*uclibmalloc_m)(int,void *) noex ;
    struct uclibmemalloc {
	uclibmalloc_m	m ;
	cvoid		*cp ;
	uclibmemalloc(cvoid *op = nullptr) noex : cp(op) { } ;
	int operator () (int,void *) noex ;
	int stdmalloc(int,void *) noex ;
	int stdvalloc(int,void *) noex ;
	int stdrealloc(int,void *) noex ;
    } ; /* end struct (uclibmemalloc) */
}


/* forward references */


/* local vaiables */


/* exported variables */


/* exported subroutines */

int uc_libmallocstrw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (sp && rpp) {
	    char	*bp{} ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = uc_libmalloc((sl+1),&bp)) >= 0) {
	        *rpp = bp ;
	        strncpy(bp,sp,sl) ;
	        bp[sl] = '\0' ;
	    } else {
		*rpp = nullptr ;
	    } /* end if (uc_libmalloc) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (uc_libmallocstrw) */

int uc_libmallocsys(int w,char **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rpp) {
	    *rpp = nullptr ;
	    if ((rs = getbufsize(w)) >= 0) {
	        char	*bp{} ;
		rl = rs ;
	        if ((rs = uc_libmalloc((rl+1),&bp)) >= 0) {
	            *rpp = bp ;
	        } /* end if */
	    } /* end if (getbufsize) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_libmallocsys) */

int uc_libmalloc(int sz,void *vp) noex {
	uclibmemalloc	lmo ;
	lmo.m = &uclibmemalloc::stdmalloc ;
	return lmo(sz,vp) ;
}
/* end subroutine (uc_libmalloc) */

int uc_libcalloc(int ne,int esize,void *vp) noex {
	cint		sz = (ne * esize) ;
	int		rs ;
	if ((rs = uc_libmalloc(sz,vp)) >= 0) {
	    memset(vp,0,sz) ;
	}
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (uc_libcalloc) */

int uc_libvalloc(int sz,void *vp) noex {
	uclibmemalloc	lmo ;
	lmo.m = &uclibmemalloc::stdvalloc ;
	return lmo(sz,vp) ;
}
/* end subroutine (uc_libvalloc) */

int uc_librealloc(cvoid *cp,int sz,void *vp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const uintptr_t	v = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & 3) == 0) {
	        uclibmemalloc	lmo(cp) ;
	        lmo.m = &uclibmemalloc::stdrealloc ;
	        rs = lmo(sz,vp) ;
	    } /* end if (aligned correctly) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_librealloc) */

int uc_libfree(cvoid *cp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const uintptr_t	v = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & 3) == 0) {
	        void	*fvp = voidp(cp) ;
	        free(fvp) ;
		rs = SR_OK ;
	    } /* end if (valid address alignment) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_libfree) */


/* local subroutines */

int uclibmemalloc::operator () (int sz,void *vp) noex {
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
/* end subroutine (uclibmemalloc::operator) */

int uclibmemalloc::stdmalloc(int sz,void *vp) noex {
	csize		msize = size_t(sz) ;
	int		rs ;
	void		**rpp = (void **) vp ;
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
/* end method (uclibmemalloc::stdmalloc) */

int uclibmemalloc::stdvalloc(int sz,void *vp) noex {
	csize		msize = size_t(sz) ;
	int		rs ;
	void		**rpp = (void **) vp ;
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
/* end method (uclibmemalloc::stdvalloc) */

int uclibmemalloc::stdrealloc(int sz,void *vp) noex {
	csize		msize = size_t(sz) ;
	void		*fvp = voidp(cp) ;
	void		**rpp = (void **) vp ;
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
}
/* end method (uclibmemalloc::stdrealloc) */


