/* umisc3 MODULE (module-implementation-unit) */
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
	umisc

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>		/* |va_list| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>		/* |eol| */

module umisc ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local structures */


/* forward references */

static int	local_pathadd(char *,int,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pathnaddw(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	if (pbuf && sp) {
	    rs = SR_INVALID ;
	    if ((plen >= 0) && (pl >= 0)) {
		rs = local_pathadd(pbuf,plen,pl,sp,sl) ;
		pl = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (pathnaddw) */

int pathnaddx(char *pbuf,int plen,int pl,int n,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (pbuf) {
	    rs = SR_INVALID ;
	    if ((plen >= 0) && (pl >= 0)) {
	        va_begin(ap,n) ;
	        for (int i = 0 ; (rs >= SR_OK) && (i < n) ; i += 1) {
		    cchar	*sp = (char *) va_arg(ap,char *) ;
		    rs = local_pathadd(pbuf,plen,pl,sp,-1) ;
		    pl = rs ;
	        } /* end for */
	        va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (pathnaddx) */


/* local subroutines */

static int local_pathadd(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if ((pl > 0) && (pbuf[pl - 1] != '/')) {
	    rs = snaddslash(pbuf,plen,pl) ;
	    pl += rs ;
	}
	if (rs >= 0) {
	    rs = snaddw(pbuf,plen,pl,sp,sl) ;
	    pl += rs ;
	}
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (local_pathadd) */


