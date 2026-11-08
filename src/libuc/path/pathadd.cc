/* pathadd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* add a component (c-string) to an existing path (c-string) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2002-07-13, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pathnaddw
	pathnaddx
	pathaddw
	pathaddx

	Description:
	This subroutine adds a new component to an existing file-path.
	These subroutines return the total length of the final path.
	The length of the supplied buffer for the added-to path is
	assumed to be MAXPATHLEN (determined dynamically).

	Synopses:
	int pathnaddw	(char *pbuf,int plen,int pl,cchar *sp) noex
	int pathnaddx	(char *pbuf,int plen,int pl,int n,cchar *sp ...) noex
	int pathaddw	(char *pbuf,int pl,cchar *sp,int sl) noex
	int pathaddx	(char *pbuf,int pl,int n,cchar *sp ...) noex

	Arguments:
	pbuf		result buffer pointer
	pl		length of result buffer that is already filled
	n		number of arguments
	sp		new-componment c-string pointer
	sl		new-componment c-string length

	Returns:
	>=0		new length of new path; total length from beginning
	<0		error (system-return)

	Notes:
	1. These subroutines return the total length of the final path
	that resides in the given path-result-buffer.  If you want
	a subroutine that returns just the added part, then check
	out the |storebuf(3uc)| facility or the |snadd(3uc)| facility,
	or simply create your own using the various |snx(3uc)|
	subroutines (and other methods also).  In the subroutines
	presented here, the length of the supplied buffer for the
	added-to path is assumed to be MAXPATHLEN (determined
	dynamically), and so is not needed as a call argument.

	Extra:
	The |pathaddw| subroutine is almost equivalent to:
	{{
	    int pathaddw(char *rbuf,int rl,cc *sp,int sl) noex {
	        int	rs ;
	        if ((rs = bufsizeget(bufsize_mp)) >= 0) {
		    cint	rlen = rs ;
	    	    if ((rs = storebuf_chr(rbuf,rlen,rl,'/')) >= 0) {
		        rl += rs ;
	                rs = storebuf_strw(rbuf,rlen,rl,sp,sl) ;
		        rl += rs ;
		    }
	        }
	        return (rs >= 0) ? rl : rs ;
	    }
	}}

	2. Implementation: With some (extreme) difficulty, these
	subroutines have been coerced to compile and link using
	only the LIBU library.  Actually, almost all of this
	functionality is already in LIBU, so why am I complaining
	so much?

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |snadd(3u)| */
#include	<aflag.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU */

#include	"pathadd.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* ?? */
import ulibvals ;			/* |ulibval(3u)| */
import deb ;				/* debugging (LIBU) */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libu::snaddslash ;		/* subroutine */
using libu::snaddw ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct initer {
	int		maxpathlen ;
	aflag		finit ;
	operator int () noex ;
    } ; /* end struct (initer) */
} /* end namespace */


/* forward references */

local int local_pathnaddw(char *,int,int,ccharp,int = -1) noex ;


/* local variables */

static initer		init ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pathnaddw(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	DEBPRINTF("ent\n") ;
	if (pbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if ((plen >= 0) && (pl >= 0)) ylikely {
		rs = local_pathnaddw(pbuf,plen,pl,sp,sl) ;
		pl = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (pathnaddw) */

int pathnaddx(char *pbuf,int plen,int pl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (pbuf) ylikely {
	    rs = SR_INVALID ;
	    if ((plen >= 0) && (pl >= 0)) ylikely {
	        va_begin(ap,n) ;
		rs = SR_OK ;
	        for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
		    ccharp sp = (ccharp) va_arg(ap,ccharp) ;
		    if (! sp) break ;
		    rs = local_pathnaddw(pbuf,plen,pl,sp) ;
		    pl = rs ;
	        } /* end for */
	        va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (pathnaddx) */

int pathaddw(char *pbuf,int pl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	DEBPRINTF("ent\n") ;
	if (pbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if (pl >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
		    cint plen = rs ;
		    rs = local_pathnaddw(pbuf,plen,pl,sp,sl) ;
		    pl = rs ;
	        } /* end if (initer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (pathaddw) */

int pathaddx(char *pbuf,int pl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (pbuf) ylikely {
	    rs = SR_INVALID ;
	    if (pl >= 0) ylikely {
	        if ((rs = init) >= 0) ylikely {
		    cint plen = rs ;
	            va_begin(ap,n) ;
	            for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
		        ccharp sp = (ccharp) va_arg(ap,ccharp) ;
			if (! sp) break ;
		        rs = local_pathnaddw(pbuf,plen,pl,sp) ;
		        pl = rs ;
	            } /* end for */
	            va_end(ap) ;
	        } /* end if (initer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (pathaddx) */


/* local subroutines */

local int local_pathnaddw(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = snaddslash(pbuf,plen,pl)) >= 0) {
	    pl += rs ;
	    rs = snaddw(pbuf,plen,pl,sp,sl) ;
	    pl += rs ;
	} /* end */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (local_pathnaddw) */

initer::operator int () noex {
    	int		rs = maxpathlen ;
	if (! finit) {
    	    if (! finit.testandset) {
	        if ((rs = ulibval.maxpathlen) >= 0) {
		    maxpathlen = rs ;
	        } /* end */
	    } /* end if (initialized once) */
	} /* end if */
	return rs ;
} /* end method (initer::operator) */


