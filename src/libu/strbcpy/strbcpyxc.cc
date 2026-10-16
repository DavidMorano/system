/* strbcpyxc SUPPORT (String-Buffer-Copy) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */

#define	CF_STPCPY	1		/* use |stpcpy(3c)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	strbcpybc
	strbcpylc
	strbcpyuc
	strbcpyfc

	Description:
	These subroutines copy a source c-string to a (uncounted)
	destination while converting the case of the characters
	(according to which subroutine is called).

	Synopsis:
	rp = *strbcpy{x}c(char *dp,int dl,cchar *src ...) noex

	Arguments:
	dp	destination result buffer pointer
	dl	destination result buffer length
	src	source c-string pointer
	rp	result pointer (pointing to resulting NUL character 
		in 'dst'

	Returns:
	-	a pointer to the NUL character in the destination buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strbcpyxc.h"


/* local defines */

#ifndef	CF_STPCPY
#define	CF_STPCPY	1		/* use |stpcpy(3c)| */
#endif


/* imported namespaces */

using libu::chtobc ;			/* subroutine */
using libu::chtolc ;			/* subroutine */
using libu::chtouc ;			/* subroutine */
using libu::chtofc ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef char (*chtoxc_f)(int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct dstspan {
	charp	dp ;
	int	dl ;
    } ; /* end struct (dstspan) */
} /* end namespace */


/* forward references */

local inline dstspan cvtcpy(chtoxc_f cvt,char *dp,int dl,cc *sp) noex {
	while (dl && *sp) {
	    *dp++ = cvt(*sp++) ;
	    dl -= 1 ;
	} /* end while */
	return { dp, dl } ;
} /* end subroutine (cvtcpy) */

local char *strbcpyxc(chtoxc_f cvt,char *dp,int dl,cchar *sp) noex {
    	if (dp && sp) ylikely {
	    dstspan ds ;
	    if (dl >= 0) {
		ds = cvtcpy(cvt,dp,dl,sp) ;
		dp = ds.dp ;
		dl = ds.dl ;
	        *dp = '\0' ;
	        if ((dl == 0) && *sp) dp = nullptr ;
	    } else {
	        while (*sp) {
	            *dp++ = cvt(*sp++) ;
	        } /* end while */
	        *dp = '\0' ;
	    } /* end if */
	} else {
	    dp = nullptr ;
	} /* end if (non-null) */
	return dp ;
} /* end subroutine (strbcpyxc) */

namespace {
    struct cvter {
	chtoxc_f	cvt ;
	charp		dp ;
	int		dl ;
	cvter(chtoxc_f ªf,charp ªdp,int ªdl) noex {
	    cvt = ªf ;
	    dp = ªdp ;
	    dl = ªdl ;
	} ; /* end ctor */
	charp operator () (int,va_list) noex ;
	charp cvtpos	(int,va_list) noex ;
	charp cvtneg	(int,va_list) noex ;
    } ; /* end struct (cvter) */
} /* end namespace */

charp cvter::cvtpos(int na,va_list ap) noex {
	dstspan ds ;
    	bool fbad = false ;
	for (int i = 0 ; i < na ; i += 1) {
	    ccp sp = (charp) va_arg(ap,charp) ;
	    if (! sp) break ;
	    ds = cvtcpy(cvt,dp,dl,sp) ;
	    dp = ds.dp ;
	    dl = ds.dl ;
	    if ((fbad = ((dl == 0) && *sp))) break ;
	} /* end for */
	*dp = '\0' ;
	if (fbad) dp = nullptr ;
	return dp ;
} /* end method (cvter:cvtpos) */

charp cvter::cvtneg(int na,va_list ap) noex {
	dstspan ds ;
	for (int i = 0 ; i < na ; i += 1) {
	    ccp sp = (charp) va_arg(ap,charp) ;
	    if (! sp) break ;
	    ds = cvtcpy(cvt,dp,-1,sp) ;
	    dp = ds.dp ;
	} /* end for */
	return dp ;
} /* end method (cvter:cvtneg) */

charp cvter::operator () (int na,va_list ap) noex {
    	char *rp{} ;
    	if (dl >= 0) {
	    rp = cvtpos(na,ap) ;
	} else {
	    rp = cvtneg(na,ap) ;
	} /* end if */
	return rp ;
} /* end method (cvter::operator) */

local char *strbcpyxcx(chtoxc_f cvt,char *dp,int dl,int na,va_list ap) noex {
    	cvter co(cvt,dp,dl) ;
	return co(na,ap) ;
} /* end subroutine */


/* local variables */

constexpr bool		f_stpcpy = CF_STPCPY ;


/* exported variables */


/* exported subroutines */

char *strbcpybc(char *dp,int dl,cchar *sp) noex {
	char		*rp = nullptr ;
	if (dp && sp) ylikely {
	    if (dl >= 0) {
		rp = strbcpyxc(chtobc,dp,dl,sp) ;
	    } else {
	        if_constexpr (f_stpcpy) {
	            rp = stpcpy(dp,sp) ;
	        } else {
	            rp = strbcpyxc(chtobc,dp,dl,sp) ;
	        } /* end */
	    } /* end */
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (strbcpybc) */

char *strbcpylc(char *dp,int dl,cchar *sp) noex {
	return strbcpyxc(chtolc,dp,dl,sp) ;
} /* end subroutine */

char *strbcpyuc(char *dp,int dl,cchar *sp) noex {
	return strbcpyxc(chtouc,dp,dl,sp) ;
} /* end subroutine */

char *strbcpyfc(char *dp,int dl,cchar *sp) noex {
	return strbcpyxc(chtofc,dp,dl,sp) ;
} /* end subroutine */

char *strbcpybcx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtobc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *strbcpylcx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtolc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *strbcpyucx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtouc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */

char *strbcpyfcx(char *dp,int dl,int na,...) noex {
    	va_list ap ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    dp = strbcpyxcx(chtofc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return dp ;
} /* end subroutine */


