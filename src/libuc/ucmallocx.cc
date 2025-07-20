/* ucmallocx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* process memory management */
/* version %I% last-modified %G% */

#define	CF_STRNCPY	1		/* try it out */

/* revision history:

	= 1998-03-85, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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
	>=0		number of bytes in allocated (system) buffer
	<0		error code (system-return)


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
	<0		error code (system-return)


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
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncpy(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"ucmallocx.h"

import libutil ;

/* local defines */

#ifndef	CF_STRNCPY
#define	CF_STRNCPY	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static inline void ourstrwcpy(char *bp,cchar *sp,int sl) noex {
	constexpr bool		f = CF_STRNCPY ;
	if_constexpr (f) {
	    strncpy(bp,sp,sl) ;
	    bp[sl] = '\0' ;
	} else {
	    while (sl-- && *sp) {
		*bp++ = *sp++ ;
	    }
	    *bp = '\0' ;
	} /* end if_constexpr (f) */
}
/* end subroutine (ourcpywstr) */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_mallocsys(int w,char **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rpp) {
	    *rpp = nullptr ;
	    if ((rs = getbufsize(w)) >= 0) {
		rl = rs ;
	        if (char *bp{} ; (rs = uc_malloc((rl+1),&bp)) >= 0) {
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
	    if (vl < 0) {
		cchar	*sp = charp(vp) ;
		vl = lenstr(sp) ;
	    }
	    if (char *bp ; (rs = uc_malloc((vl + 1),&bp)) >= 0) {
	        memcopy(bp,vp,vl) ;
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
	    sl = lenstr(sp,sl) ;
	    if (char *bp ; (rs = uc_malloc((sl + 1),&bp)) >= 0) {
	        *rpp = bp ;
	        ourstrwcpy(bp,sp,sl) ;
	    } else {
	        *rpp = nullptr ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (uc_mallocstrw) */


