/* uclibmemalloc (3uc) */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* memory allocation facility (for library use) */


/* revision history:

	= 1998-03-85, David A­D­ Morano
	This subroutine was originally written.

	= 2018-09-26, David A-D- Morano
	I brought |uc_libcalloc(3uc)| in line w/ the standard for
	|calloc(3c)|.  I never used this myself in 40 years, so I
	never missed it!

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is the low-level component of the MEMALLOC facility.
	These subroutines need to be able to be interposed upon,
	so they have to be in their own compilation (object) image.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"uclibmemalloc.h"


/* local defines */


/* external subroutines */


/* local structures */

namespace {
    struct uclibmemalloc ;
    typedef int (uclibmemalloc::*mem_f)(int,void *) noex ;
    struct uclibmemalloc {
	mem_f		m ;
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


/* exported subroutines */

int uc_libmallocstrw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (sp && rpp) {
	    char	*bp ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = uc_libmalloc((sl+1),&bp)) >= 0) {
	        *rpp = bp ;
	        strncpy(bp,sp,sl) ;
	        bp[sl] = '\0' ;
	    } else {
		*rpp = nullptr ;
	    } /* end if (uc_libmalloc) */
	} /* end if (non-null) */
	return rs ;
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

int uc_libmalloc(int size,void *vp) noex {
	uclibmemalloc	lmo ;
	lmo.m = &uclibmemalloc::stdmalloc ;
	return lmo(size,vp) ;
}
/* end subroutine (uc_libmalloc) */

int uc_libcalloc(int nelem,int esize,void *vp) noex {
	cint		size = (nelem*esize) ;
	int		rs ;
	if ((rs = uc_libmalloc(size,vp)) >= 0) {
	    memset(vp,0,size) ;
	}
	return rs ;
}
/* end subroutine (uc_libcalloc) */

int uc_libvalloc(int size,void *vp) noex {
	uclibmemalloc	lmo ;
	lmo.m = &uclibmemalloc::stdvalloc ;
	return lmo(size,vp) ;
}
/* end subroutine (uc_libvalloc) */

int uc_librealloc(cvoid *cp,int size,void *vp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const ulong	v = ulong(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & 3) == 0) {
	        uclibmemalloc	lmo(cp) ;
	        lmo.m = &uclibmemalloc::stdrealloc ;
	        rs = lmo(size,vp) ;
	    } /* end if (aligned correctly) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_librealloc) */

int uc_libfree(cvoid *cp) noex {
	int		rs = SR_FAULT ;
	if (cp) {
	    const ulong	v = ulong(cp) ;
	    rs = SR_BADFMT ;
	    if ((v & 3) == 0) {
	        void	*fvp = (void *) cp ;
	        free(fvp) ;
		rs = SR_OK ;
	    } /* end if (valid address alignment) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_libfree) */


/* local subroutines */

int uclibmemalloc::operator () (int size,void *vp) noex {
	int     	rs = SR_FAULT ;
	if (vp) {
	    rs = SR_INVALID ;
	    if (size > 0) {
	        int     to_again = utimeout[uto_again] ;
	        int     to_nomem = utimeout[uto_nomem] ;
	        bool    f_exit = false ;
	        repeat {
	            if ((rs = (this->*m)(size,vp)) < 0) {
	                switch (rs) {
	                case SR_AGAIN:
	                    if (to_again-- > 0) {
			        msleep(1000) ;
	                    } else {
			        f_exit = true ;
	                    }
	                    break ;
	                case SR_NOMEM:
	                    if (to_nomem-- > 0) {
			        msleep(1000) ;
	                    } else {
			        f_exit = true ;
	                    }
	                    break ;
	                case SR_INTR:
	                    break ;
	                default:
	                    f_exit = true ;
	                    break ;
	                } /* end switch */
	            } /* end if (error) */
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid size) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uclibmemalloc::operator) */

int uclibmemalloc::stdmalloc(int size,void *vp) noex {
	size_t		msize = size_t(size) ;
	int		rs ;
	void		**rpp = (void **) vp ;
	void		*rp ;
	errno = 0 ;
	if ((rp = malloc(msize))) {
	    rs = size ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
}
/* end method (uclibmemalloc::stdmalloc) */

int uclibmemalloc::stdvalloc(int size,void *vp) noex {
	size_t		msize = size_t(size) ;
	int		rs ;
	void		**rpp = (void **) vp ;
	void		*rp ;
	errno = 0 ;
	if ((rp = valloc(msize))) {
	    rs = size ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
}
/* end method (uclibmemalloc::stdvalloc) */

int uclibmemalloc::stdrealloc(int size,void *vp) noex {
	size_t		msize = size_t(size) ;
	int		rs ;
	void		**rpp = (void **) vp ;
	void		*fvp = (void *) cp ;
	void		*rp ;
	errno = 0 ;
	if ((rp = realloc(fvp,msize))) {
	    rs = size ;
	    *rpp = rp ;
	} else {
	    rs = (- errno) ;
	    *rpp = nullptr ;
	}
	return rs ;
}
/* end method (uclibmemalloc::stdrealloc) */


