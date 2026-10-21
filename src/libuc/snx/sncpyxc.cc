/* sncpyxc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	sncpy{x}c

	Description:
	These subroutines construct a single string (into a
	caller-supplied counted buffer) from a single specificed
	c-string.  But a character-case conversion is available.
	Available character-case conversions are:
	+ none (straight through copy)
	+ convert to lower-case
	+ convert to upper-case
	+ convert to folded-case

	Synopsis:
	int sncpy{x}c(char *rbuf,int rlen,cchar *sp) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	sp		source c-string to copy-convert

	Returns:
	>=0		number of bytes copies (coverted)
	<0		error (system-return)[

	See-also:
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |chtoxc(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"sncpyxc.h"


/* local defines */


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


/* forward references (local subroutine-templates) */

local int sncpyxc(chtoxc_f cvt,char *dbuf,int dlen,cchar *sp) noex {
    	int		rs = SR_FAULT ;
	if (dbuf && sp) ylikely {
	    int		i = 0 ; /* used-afterwards */
	    char	*dp = dbuf ;
	    rs = SR_OVERFLOW ;
	    if (dlen < 0) dlen = INT_MAX ;
	    while ((i < dlen) && sp[i]) {
	        *dp++ = cvt(sp[i++]) ;
	    } /* end while */
	    *dp = eos ;
	    if (sp[i] == eos) {
	        rs = intconv(dp - dbuf) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine-template (sncpyxc) */

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
        int operator ()	(int,va_list) noex ;
        int cvtcpy	(cchar *) noex ;
    } ; /* end struct (cvter) */
} /* end namespace */

int cvter::operator () (int na,va_list ap) noex {
    	int		rs = SR_OK ;
	char		*bp = dp ;
	for (int i = 0 ; (rs >= 0) && (i < na) ; i += 1) {
	    ccp sp = (charp) va_arg(ap,charp) ;
	    rs = cvtcpy(sp) ;
	} /* end for */
	if (rs >= 0) rs = intconv(dp - bp) ;
	return rs ;
} /* end method (cvter::operator) */

int cvter::cvtcpy(cchar *sp) noex {
    	int		rs = SR_OK ;
	if (dl >= 0) {
	    int dlx ; /* used-afterwards */
    	    for (dlx = dl ; dlx && *sp ; dlx -= 1) {
	        *dp++ = cvt(*sp++) ;
	    } /* end for */
	    dl = dlx ;
	    if ((dl == 0) && *sp) rs = SR_OVERFLOW ;
	} else {
    	    while (*sp) {
	        *dp++ = cvt(*sp++) ;
	    } /* end while */
	} /* end if */
	return rs ;
} /* end method (cvter::cvtcpy) */

local int sncpyxcx(chtoxc_f cvt,char *dp,int dl,int na,va_list ap) noex {
    	cvter co(cvt,dp,dl) ;
	return co(na,ap) ;
} /* end subroutine (sncpyxcx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sncpybc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc(chtobc,dp,dl,s1) ;
} /* end subroutine */

int sncpylc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc(chtolc,dp,dl,s1) ;
} /* end subroutine */

int sncpyuc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc(chtouc,dp,dl,s1) ;
} /* end subroutine */

int sncpyfc(char *dp,int dl,cchar *s1) noex {
	return sncpyxc(chtofc,dp,dl,s1) ;
} /* end subroutine */

int sncpybcx(char *dp,int dl,int na,...) noex {
    	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    rs = sncpyxcx(chtobc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return rs ;
} /* end subroutine */

int sncpylcx(char *dp,int dl,int na,...) noex {
    	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    rs = sncpyxcx(chtolc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return rs ;
} /* end subroutine */

int sncpyucx(char *dp,int dl,int na,...) noex {
    	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    rs = sncpyxcx(chtouc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return rs ;
} /* end subroutine */

int sncpyfcx(char *dp,int dl,int na,...) noex {
    	va_list		ap ;
	int		rs = SR_FAULT ;
	if (dp) ylikely {
	    va_begin(ap,na) ;
	    rs = sncpyxcx(chtofc,dp,dl,na,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
    	return rs ;
} /* end subroutine */


