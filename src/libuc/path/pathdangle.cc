/* pathdangle SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the sub-path of a dangling soft-link */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This was written as part of the FILECHECK utility.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pathdangle

	Description:
	This subroutine finds a sub-path within a larger path
	(consisting of a counted c-string) that is a dangling
	soft-link.

	Synopsis:
	int pathdangle(cchar *pbuf,int plen) noex

	Arguments:
	pbuf		source path to examine pointer
	plen		source path to examine length

	Returns:
	>=0		length of cleaned up path
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<aflag.hh>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DPRINTF(3u)| */

#include	"pathdangle.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import deb ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif

#define	VARPATHDANGLE	"PATHDANGLE_DEBFN"


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct initer {
	aflag		finit{} ;
	operator int () noex ;
    } ; /* end struct (initer) */
} /* end namespace */


/* local structures */


/* forward references */

local int pathdangler(cchar *sp,int sl) noex ;
[[maybe_unused]]
local int	nextname(cchar *,int,cchar **) noex ;


/* local variables */

static initer		init ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pathdangle(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	    rs = SR_OK ;
	    if (sl > 0) ylikely {
	        if (static cint rsi = init ; (rs = rsi) >= 0) {
	            rs = pathdangler(sp,sl) ;
		    len = rs ;
	        } /* end if (initer) */
	    } /* end if (non-empty) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pathdangle) */


/* local subroutines */

local int pathdangler(cchar *sp,int sl) noex {
    	cint		plen = sl ;
    	int		rs = SR_OK ;
	int		rs1 ;
	int		pl = 0 ; /* return-value */
	(void) sp ;
	(void) sl ;
	if (char *pbuf ; (rs = libmem.mall((plen + 1),&pbuf)) >= 0) {
	    int	cl ;
	    cchar *cp ;
	    while ((rs >= 0) && ((cl = nextname(sp,sl,&cp)) >= 0)) {
		if ((
	    } /* end while */
	    rs1 = libmem.free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (pathdangler) */

local int nextname(cchar *sp,int sl,cchar **rpp) noex {
    	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	bool		f_len = (sl >= 0) ;
	while (((! f_len) || (sl > 0)) && (*sp == '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	rp = sp ;
	while ((((! f_len) && *sp) || (sl > 0)) && (*sp != '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	rl = intconv(sp - rp) ;
	if (rpp) {
	    *rpp = rp ;
	} /* end */
	return rl ;
} /* end subroutine (nextname) */

initer::operator int () noex {
    	int		rs = SR_OK ;
	if_constexpr (f_debug) {
	    if (! finit.testandset) {
	        if (ccharp dfn = getenv(VARPATHDANGLE)) {
		    if (cint rs1 = DEBOPEN(dfn) ; rs1 >= 0) {
		        DEBPRINTF("debinit fd=%d\n",rs1) ;
		    } /* end if (DEBOPEN) */
	        } /* end if (getenv) */
	    } /* end if (needed) */
	} /* end if_constexpr (f_debug) */
	return rs ;
} /* end method (initer::operator) */


