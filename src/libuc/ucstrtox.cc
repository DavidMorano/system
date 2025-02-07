/* ucstrtox SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* string-digit conversion functions */
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
	I had to do some cleanup work on the case of concerting to
	a reular integer (which is 32-bits on most platforms
	now-a-days) so that its overflow and underflow return statis
	is correct.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<stdintx.h>
#include	<localmisc.h>

#include	"strtox.h"		/* <- the money shot */


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> 
constexpr static inline int nbits(T) noex {
    	return (szof(T) * CHAR_BIT) ;
}

template<typename T>
constexpr static inline bool bit(T v,int n) noex {
	return !!((v >> n) & 1) ;
}

template<typename T>
void strtox(cchar *,char **,int,T *rp) noex {
	*rp = 0 ;
}

template<>
void strtox(cchar *sp,char **epp,int b,int *rp) noex {
	clong	v = strtol(sp,epp,b) ;
	*rp = int(v) ;
	if (errno == 0) {
	    cint	n = nbits(v) ;
	    {
	        cbool	f_neg = bit(v,(n-1)) ;
		if (f_neg) {	/* test negative value */
	    	    ulong	uv = ulong(v) ;
		    uv = (~ uv) ;
	            uv >>= (n/2) ;
		    if (uv || (! bit(v,((n/2)-1)))) {
			errno = ERANGE ;
		    }
		} else {	/* test poitive value */
	    	    ulong	uv = ulong(v) ;
	            uv >>= (n/2) ;
		    if (uv || bit(v,((n/2)-1))) {
			errno = ERANGE ;
		    }
		}
	    } /* end block */
	} /* end if (no error so far) */
}

template<>
void strtox(cchar *sp,char **epp,int b,long *rp) noex {
	*rp = strtol(sp,epp,b) ;
}

template<>
void strtox(cchar *sp,char **epp,int b,longlong *rp) noex {
	*rp = strtoxll(sp,epp,b) ;
}

template<>
void strtox(cchar *sp,char **epp,int b,uint *rp) noex {
	ulong	uv = strtoul(sp,epp,b) ;
	*rp = uint(uv) ;
	if (errno == 0) {
	    cint	n = nbits(uv) ;
	    uv >>= (n/2) ;
	    if (uv) {
		errno = ERANGE ;
	    }
	}
}

template<>
void strtox(cchar *sp,char **epp,int b,ulong *rp) noex {
	*rp = strtoul(sp,epp,b) ;
}

template<>
void strtox(cchar *sp,char **epp,int b,ulonglong *rp) noex {
	*rp = strtoxull(sp,epp,b) ;
}

template<typename T>
int ucstrtox(cchar *sp,cchar **epp,int b,T *rp) noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	    char	*endp = nullptr ;
	    errno = 0 ;
	    strtox(sp,&endp,b,rp) ;
	    if (epp) *epp = endp ;
	    rs = (errno != 0) ? (- errno) : (endp - sp) ;
	}
	return rs ;
}
/* end subroutine-template (ucstrtox) */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_strtoi(cchar *sp,cchar **epp,int b,int *rp) noex {
	return ucstrtox(sp,epp,b,rp) ;
}
/* end subroutine (uc_strtoi) */

int uc_strtol(cchar *sp,cchar **epp,int b,long *rp) noex {
	return ucstrtox(sp,epp,b,rp) ;
}
/* end subroutine (uc_strtol) */

int uc_strtoll(cchar *sp,cchar **epp,int b,longlong *rp) noex {
	return ucstrtox(sp,epp,b,rp) ;
}
/* end subroutine (uc_strtoll) */

int uc_strtoui(cchar *sp,cchar **epp,int b,uint *rp) noex {
	return ucstrtox(sp,epp,b,rp) ;
}
/* end subroutine (uc_strtoui) */

int uc_strtoul(cchar *sp,cchar **epp,int b,ulong *rp) noex {
	return ucstrtox(sp,epp,b,rp) ;
}
/* end subroutine (uc_strtoul) */

int uc_strtoull(cchar *sp,cchar **epp,int b,ulonglong *rp) noex {
	return ucstrtox(sp,epp,b,rp) ;
}
/* end subroutine (uc_strtoull) */


