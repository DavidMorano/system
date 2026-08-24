/* ucstrtox SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* integer string-digit conversion functions */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucstrtox

	Description:
	I make an interface to the standard-C library for most
	types.  But I have to roll my own for the |{x}longlong| type
	(|{x}int64_t|).  I have six exported subroutines from this
	module (3 types X 2 signs).

	Notes:
	I had to do some cleanup work on the case of converting to
	a regular integer (which is 32-bits on most platforms
	now-a-days) so that its overflow and underflow return status
	is correct.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<strtox.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucstrtox.h"		/* <- the money shot */


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> constexpr local inline int nbits(T)		noex {
    	return (szof(T) * CHAR_BIT) ;
} /* end subroutine-template */

template<typename T> constexpr local inline bool bit(T v,int n)		noex {
	return bool((v >> n) & 1) ;
} /* end subroutine-template */

template<typename T> local void strtox(cchar *,char **,int,T *rp)	noex {
	*rp = 0 ;
} /* end subroutine-template */

/* |int| */
template<> void strtox(cchar *sp,char **epp,int b,int *rp)		noex {
	clong	v = strtol(sp,epp,b) ;
	*rp = int(v) ;
	if (errno == 0) {
	    cint	n = nbits(v) ;
	    {
	        cbool	f_neg = bit(v,(n-1)) ;
		if (f_neg) {	/* test negative value */
	    	    ulong	uv = ulong(v) ;
		    uv = (compl uv) ;
	            uv >>= (n/2) ;
		    if (uv || (! bit(v,((n/2)-1)))) {
			errno = ERANGE ;
			*rp = INT_MIN ;
		    }
		} else {	/* test poitive value */
	    	    ulong	uv = ulong(v) ;
	            uv >>= (n/2) ;
		    if (uv || bit(v,((n/2)-1))) {
			errno = ERANGE ;
			*rp = INT_MAX ;
		    }
		} /* end if */
	    } /* end block */
	} /* end if (no error so far) */
} /* end subroutine-template (strtox) */

/* |long| */
template<> void strtox(cchar *sp,char **epp,int b,long *rp)		noex {
	*rp = strtol(sp,epp,b) ;
} /* end */

/* |longlong| */
template<> void strtox(cchar *sp,char **epp,int b,longlong *rp)		noex {
	*rp = strtoxll(sp,epp,b) ;
} /* end */

/* |uint| */
template<> void strtox(cchar *sp,char **epp,int b,uint *rp)		noex {
	ulong	uv = strtoul(sp,epp,b) ;
	*rp = uint(uv) ;
	if (errno == 0) {
	    cint	n = nbits(uv) ;
	    uv >>= (n/2) ;
	    if (uv) {
		errno = ERANGE ;
		*rp = UINT_MAX ;
	    }
	} /* end if (not-error) */
} /* end subroutine-template (strtox) */

/* |ulong| */
template<> void strtox(cchar *sp,char **epp,int b,ulong *rp)		noex {
	*rp = strtoul(sp,epp,b) ;
} /* end subroutine-template */

/* |ulonglong| */
template<> void strtox(cchar *sp,char **epp,int b,ulonglong *rp)	noex {
	*rp = strtoxull(sp,epp,b) ;
} /* end subroutine-template */

template<typename T>
local sysret_t ucstrtox(cchar *sp,cchar **epp,int b,T *rp)		noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	    char	*endp = nullptr ;
	    errno = 0 ;
	    strtox(sp,&endp,b,rp) ;
	    if (epp) *epp = endp ;
	    rs = (errno) ? (neg errno) : intconv(endp - sp) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine-template (ucstrtox) */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_strtosi(cchar *sp,cchar **epp,int b,int *rp)		noex {
	return ucstrtox(sp,epp,b,rp) ;
} /* end subroutine (uc_strtoi) */

int uc_strtosl(cchar *sp,cchar **epp,int b,long *rp)		noex {
	return ucstrtox(sp,epp,b,rp) ;
} /* end subroutine (uc_strtol) */

int uc_strtosll(cchar *sp,cchar **epp,int b,longlong *rp)	noex {
	return ucstrtox(sp,epp,b,rp) ;
} /* end subroutine (uc_strtoll) */

int uc_strtoui(cchar *sp,cchar **epp,int b,uint *rp)		noex {
	return ucstrtox(sp,epp,b,rp) ;
} /* end subroutine (uc_strtoui) */

int uc_strtoul(cchar *sp,cchar **epp,int b,ulong *rp)		noex {
	return ucstrtox(sp,epp,b,rp) ;
} /* end subroutine (uc_strtoul) */

int uc_strtoull(cchar *sp,cchar **epp,int b,ulonglong *rp)	noex {
	return ucstrtox(sp,epp,b,rp) ;
} /* end subroutine (uc_strtoull) */


