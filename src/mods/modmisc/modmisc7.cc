/* modmisc7 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Miscellaneous support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-19, David A­D­ Morano
	This code was originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Module:
	modmisc

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module modmisc ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* local structures */

namespace {
    struct adder {
	char	*rbuf ;
	int	rlen ;
	int	rl ;
	adder(charp ªrb,int ªrl,int ªl) noex {
	    rbuf	= ªrb ;
	    rlen	= ªrl ;
	    rl		= ªl ;
	} ; /* end ctor */
	int operator ()	(cchar *,int) noex ;
	int aslash	() noex ;
	int astr	(cchar *,int) noex ;
    } ; /* end struct (adder) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snaddpath(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (pbuf && sp) ylikely {
	    rs = SR_INVALID ;
	    if ((plen >= 0) && (pl >= 0)) ylikely {
		adder ao(pbuf,plen,pl) ;
		rs = ao(sp,sl) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snaddpath) */


/* local subroutines */

int adder::operator () (cchar *sp,int sl) noex {
    	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = aslash()) >= 0) {
	    len += rs ;
	    rs = astr(sp,sl) ;
	    len += rs ;
	    rbuf[rl] = eos ;
	} /* end if (aslash) */
	return (rs >= 0) ? len : rs ;
} /* end method (adder::operator) */

int adder::aslash() noex {
	int		rs = SR_OK ;
    	int		len = 0 ;
	if ((rl > 0) && (rbuf[rl - 1] != '/')) {
	    rs = SR_OVERFLOW ;
	    if (rl < rlen) {
		rs = SR_OK ;
	        rbuf[rl++] = '/' ;
	        len += 1 ;
	    } /* end if (good) */
	} /* end if (needed) */
	return (rs >= 0) ? len : rs ;
} /* end method (adder::operator) */

int adder::astr(cchar *sp,int sl) noex {
	int		rs = SR_OVERFLOW ;
	ccharp		ep = (rbuf + rlen) ;
	charp		bp = (rbuf + rl) ;
	if (sl >= 0) {
	    while ((bp < ep) && sl && *sp) {
		*bp++ = *sp++ ;
		sl -= 1 ;
	    } /* end while */
	    if ((sl == 0) || (*sp == eos)) {
		rs = intconv(bp - (rbuf + rl)) ;
		rl += rs ;
	    } /* end if (good) */
	} else {
	    while ((bp < ep) && *sp) {
		*bp++ = *sp++ ;
	    } /* end while */
	    if (*sp == eos) {
		rs = intconv(bp - (rbuf + rl)) ;
		rl += rs ;
	    } /* end if (good) */
	} /* end if */
	return rs ;
} /* end method (adder::operator) */


