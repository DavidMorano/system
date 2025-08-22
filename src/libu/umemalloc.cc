/* umemalloc SUPPORT (3uc) */
/* charset=ISO8859-1 */
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

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct umember ;
    typedef int (umember::*umember_m)(int,void *) noex ;
    struct umember {
	umember_m	m ;
	void		*cp ;		/* constant-void-pointer */
	umember(void *op = nullptr) noex : cp(op) { } ;
	int operator () (int,void *) noex ;
	int stdmalloc(int,void *) noex ;
	int stdvalloc(int,void *) noex ;
	int stdrealloc(int,void *) noex ;
    } ; /* end struct (umember) */
}


/* forward references */


/* local vaiables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int umemallocstrw(cchar *sp,int µsl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		bl = 0 ; /* return-value */
	if (sp && rpp) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        if (char *bp ; (rs = umemalloc((sl + 1),&bp)) >= 0) {
		    bl = sl ;
	            *rpp = bp ;
	            strncpy(bp,sp,sl) ;
	            bp[bl] = '\0' ;
	        } else {
		    *rpp = nullptr ;
	        } /* end if (umemalloc) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? bl : rs ;
    } /* end subroutine (umemallocstrw) */
    int umemalloc(int sz,void *vp) noex {
	umember	lmo ;
	lmo.m = &umember::stdmalloc ;
	return lmo(sz,vp) ;
    } /* end subroutine (umemalloc) */
    int umemcalloc(int ne,int esz,void *vp) noex {
	cint		sz = (ne * esz) ;
	int		rs ;
	if ((rs = umemalloc(sz,vp)) >= 0) {
	    memset(vp,0,sz) ;
	}
	return (rs >= 0) ? sz : rs ;
    } /* end subroutine (ucalloc) */
    int umemvalloc(int sz,void *vp) noex {
	umember	lmo ;
	lmo.m = &umember::stdvalloc ;
	return lmo(sz,vp) ;
    } /* end subroutine (uvalloc) */
    int umemrealloc(void *cp,int sz,void *vp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const uintptr_t	v = uintptr_t(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & 3) == 0) {
	        umember	lmo(cp) ;
	        lmo.m = &umember::stdrealloc ;
	        rs = lmo(sz,vp) ;
	    } /* end if (aligned correctly) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (urealloc) */
    int umemfree(void *cp) noex {
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
    } /* end subroutine (umemfree) */
    int umemrsfree(int rs,void *p) noex {
	if (p) {
    	    if (cint rs1 = umemfree(p) ; rs >= 0) {
		rs = rs1 ;
	    }
	} else {
    	    if (rs >= 0) rs = SR_FAULT ;
	}
	return rs ;
    } /* end subroutine (ursfree) */
} /* end namespace (libu) */


/* local subroutines */

int umember::operator () (int sz,void *vp) noex {
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
/* end subroutine (umember::operator) */

int umember::stdmalloc(int sz,void *vp) noex {
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
/* end method (umember::stdmalloc) */

int umember::stdvalloc(int sz,void *vp) noex {
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
/* end method (umember::stdvalloc) */

int umember::stdrealloc(int sz,void *vp) noex {
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
/* end method (umember::stdrealloc) */


