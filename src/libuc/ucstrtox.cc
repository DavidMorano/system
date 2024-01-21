/* ucstrtox SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	I make an interface to the standard-C library for most
	types.  But I have to roll my own for the |xlonglong| type
	(|xint64_t|).  I have six exported subroutines from this
	module (3 types X 2 signs).

	Notes:
	I had to do some cleanup work on the case of concerting to
	a reular integer (which is 32-bits on most platforms
	now-a-days) so that its overflow and underflow return statis
	is correct.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cerrno>
#include	<climits>
#include	<cstdlib>
#include	<usystem.h>
#include	<stdintx.h>
#include	<localmisc.h>
#include	<strtoxlonglong.h>


/* local namespaces */


/* local typedefs */


/* local subroutine-templates */

template<typename T>
static inline bool bit(T v,int n) noex {
	return !!((v >> n) & 1) ;
}

template<typename T>
void strtox(cchar *,char **,int,T *rp) noex {
	*rp = 0 ;
}

template<>
void strtox(cchar *sp,char **epp,int b,int *rp) noex {
	clong	v = strtol(sp,epp,b) ;
	*rp = v ;
	if (errno == 0) {
	    cint	n = sizeof(ulong) ;
	    ulong	uv = ulong(v) ;
	    uv >>= (n>>1) ;
	    if (uv) {
	        errno = ERANGE ;
	    } else {
	        cbool	f_neg = bit(v,(n-1)) ;
		if ((!f_neg) && bit(v,((n/2)-1))) errno = ERANGE ;
	    }
	} /* end block */
}

template<>
void strtox(cchar *sp,char **epp,int b,long *rp) noex {
	*rp = strtol(sp,epp,b) ;
}

template<>
void strtox(cchar *sp,char **epp,int b,longlong *rp) noex {
	*rp = strtolonglong(sp,epp,b) ;
}

template<>
void strtox(cchar *sp,char **epp,int b,uint *rp) noex {
	ulong	uv = strtoul(sp,epp,b) ;
	*rp = uv ;
	if (errno == 0) {
	    cint	n = sizeof(ulong) ;
	    uv >>= (n/2) ;
	    if (uv) errno = ERANGE ;
	}
}

template<>
void strtox(cchar *sp,char **epp,int b,ulong *rp) noex {
	*rp = strtoul(sp,epp,b) ;
}

template<>
void strtox(cchar *sp,char **epp,int b,ulonglong *rp) noex {
	*rp = strtoulonglong(sp,epp,b) ;
}

template<typename T>
int ucstrtox(cchar *sp,cchar **epp,int b,T *rp) noex {
	int		rs = SR_FAULT ;
	if (sp && rp) {
	    char	*endp = NULL ;
	    errno = 0 ;
	    strtox(sp,&endp,b,rp) ;
	    if (epp) *epp = (cchar *) endp ;
	    rs = (errno != 0) ? (- errno) : (endp-sp) ;
	}
	return rs ;
}
/* end subroutine-template (ucstrtox) */


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


