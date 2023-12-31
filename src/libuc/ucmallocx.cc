/* ucmallocx SUUPORT */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* version %I% last-modified %G% */

#define	CF_STRNCPY	1		/* try it out */

/* revision history:

	= 1998-03-85, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_mallocsys

	Description:
	This subroutine is similar to |uc_malloc(3uc)| except that
	it takes a string argument and copies it into the newly
	allocated space.

	Synopsis:
	int uc_mallocsys(int w,void **rpp) noex

	Arguments:
	w		which type of system bffer
	rpp		resulting pointer

	Returns:
	>=0		nominally -> (strlen(string) + 1)
	<0		error


	Name:
	uc_mallocbuf

	Description:
	This subroutine is similar to |uc_malloc(3uc)| except that
	it takes a string argument and copies it into the newly
	allocated space.

	Synopsis:
	int uc_mallocbuf(cvoid *sp,int sl,void **rpp) noex

	Arguments:
	sp		string pointer
	sl		string length
	rpp		resulting pointer

	Returns:
	>=0		length of allocated space
	<0		error


	Name:
	uc_mallocstrw

	Description:
	This subroutine is similar to |uc_malloc(3uc)| except that
	it takes a string argument and copies it into the newly
	allocated space.

	Synopsis:
	int uc_mallocstrw(cchar *sp,int sl,rpp) noex

	Arguments:
	sp		string pointer
	sl		string length
	rpp		resulting pointer

	Returns:
	>=0		nominally -> strnlen(sp,sl)
	<0		error


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"ucmallocx.h"


/* local defines */

#ifndef	CF_STRNCPY
#define	CF_STRNCPY	0
#endif


/* external subroutines */


/* local variables */


/* local subroutines */

static inline void ourstrwcpy(char *bp,cchar *sp,int sl) noex {
	constexpr bool		f = CF_STRNCPY ;
	if constexpr (f) {
	    strncpy(bp,sp,sl) ;
	    bp[sl] = '\0' ;
	} else {
	    while (sl-- && *sp) *bp++ = *sp++ ;
	    *bp = '\0' ;
	}
}
/* end subroutine (ourcpywstr) */


/* exported subroutines */

int uc_mallocsys(int w,char **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rpp) {
	    *rpp = nullptr ;
	    if ((rs = getbufsize(w)) >= 0) {
	        char	*bp{} ;
		rl = rs ;
	        if ((rs = uc_malloc((rl+1),&bp)) >= 0) {
	            *rpp = bp ;
	        } /* end if */
	    } /* end if (getbufsize) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (uc_mallocsys) */

int uc_mallocbuf(cvoid *vp,int vl,cvoid **vrpp) noex {
	int	rs = SR_FAULT ;
	cchar	**rpp = (cchar **) vrpp ;
	if (vp && rpp) {
	    char	*bp{} ;
	    if (vl < 0) {
		cchar	*sp = (cchar *) vp ;
		vl = strlen(sp) ;
	    }
	    if ((rs = uc_malloc((vl+1),&bp)) >= 0) {
	        memcpy(bp,vp,vl) ;
	        bp[vl] = '\0' ;
	        *rpp = bp ;
	    } else {
	        *rpp = nullptr ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (uc_mallocbuf) */

int uc_mallocstrw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (sp && rpp) {
	    char	*bp{} ;
	    *rpp = nullptr ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = uc_malloc((sl+1),&bp)) >= 0) {
	        *rpp = bp ;
	        ourstrwcpy(bp,sp,sl) ;
	    } else {
	        *rpp = nullptr ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (uc_mallocstrw) */


