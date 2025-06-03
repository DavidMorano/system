/* umemalloc SUPPORT (3uc) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* memory allocation facility (for library use) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This code was originally written.

	= 2018-09-26, David A-D- Morano
	I brought |ucalloc(3uc)| in line w/ the standard for
	|calloc(3c)|.  I never used this myself in 40 years, so I
	never missed it!

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uclbmemalloc

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
#include	<cstring>		/* |strncpy(3c)| */
#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<utimeout.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"umemalloc.hh"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct umemalloc ;
    typedef int (umemalloc::*uclibmalloc_m)(int,void *) noex ;
    struct umemalloc {
	uclibmalloc_m	m ;
	void		*cp ;		/* constant-void-pointer */
	umemalloc(void *op = nullptr) noex : cp(op) { } ;
	int operator () (int,void *) noex ;
	int stdmalloc(int,void *) noex ;
	int stdvalloc(int,void *) noex ;
	int stdrealloc(int,void *) noex ;
    } ; /* end struct (umemalloc) */
}


/* forward references */


/* local vaiables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int umallocstrw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (sp && rpp) {
	    if (sl < 0) sl = xstrlen(sp) ;
	    if (char *bp ; (rs = umalloc((sl + 1),&bp)) >= 0) {
	        *rpp = bp ;
	        strncpy(bp,sp,sl) ;
	        bp[sl] = '\0' ;
	    } else {
		*rpp = nullptr ;
	    } /* end if (umalloc) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
    } /* end subroutine (umallocstrw) */
    int umalloc(int sz,void *vp) noex {
	umemalloc	lmo ;
	lmo.m = &umemalloc::stdmalloc ;
	return lmo(sz,vp) ;
    } /* end subroutine (umalloc) */
    int ucalloc(int ne,int esz,void *vp) noex {
	cint		sz = (ne * esz) ;
	int		rs ;
	if ((rs = umalloc(sz,vp)) >= 0) {
	    memset(vp,0,sz) ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end subroutine (ucalloc) */
    int uvalloc(int sz,void *vp) noex {
	umemalloc	lmo ;
	lmo.m = &umemalloc::stdvalloc ;
	return lmo(sz,vp) ;
    } /* end subroutine (uvalloc) */
    int urealloc(void *cp,int sz,void *vp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const uintptr_t	v = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & 3) == 0) {
	        umemalloc	lmo(cp) ;
	        lmo.m = &umemalloc::stdrealloc ;
	        rs = lmo(sz,vp) ;
	    } /* end if (aligned correctly) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (urealloc) */
    int ufree(void *cp) noex {
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
    } /* end subroutine (ufree) */
    int rslibfree(int rs,void *p) noex {
	if (p) {
    	    if (cint rs1 = ufree(p) ; rs >= 0) {
		rs = rs1 ;
	    }
	} else {
    	    if (rs >= 0) rs = SR_FAULT ;
	}
	return rs ;
    } /* end subroutine (rslibfree) */
} /* end namespace (libu) */


/* local subroutines */

int umemalloc::operator () (int sz,void *vp) noex {
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
/* end subroutine (umemalloc::operator) */

int umemalloc::stdmalloc(int sz,void *vp) noex {
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
/* end method (umemalloc::stdmalloc) */

int umemalloc::stdvalloc(int sz,void *vp) noex {
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
/* end method (umemalloc::stdvalloc) */

int umemalloc::stdrealloc(int sz,void *vp) noex {
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
}
/* end method (umemalloc::stdrealloc) */


