/* libutil1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Miscellaneous support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code, originally back around 1992 (of which there
	is quite a bit -- like almost all of it).  I will refrain
	from naming names (at least at this time).

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	libutil

	Description:
	This module provides miscellaneous UNIX® or other
	common (library oriented) subroutines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>		/* |va_list(3c)| */
#include	<cstring>		/* |memset(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>		/* |COLUMNS| */

module libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int memclearer(void *op,int sz) noex {
    	int	rs = SR_FAULT ;
    	if (op) {
	    rs = SR_INVALID ;
	    if (sz >= 0) {
		rs = sz ;
	        if (sz > 0) {
    	            csize osize = size_t(sz) ;
    	            memset(op,0,osize) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (memclearer) */

    int loadstrs(cc **strs,int n,...) noex {
    	va_list		ap ;
	int		i = -1 ; /* return-value */
	if (strs) {
	    va_begin(ap,n) ;
	    for (i = 0 ; i < n ; i += 1) {
		cc *cp = (ccharp) va_arg(ap,charp) ;
		strs[i] = cp ;
	    } /* end for */
	    strs[i] = nullptr ;
	    va_end(ap) ;
	}
	return i ;
    } /* end subroutine (loadstrs) */

int lenstrline(cchar *sp,int sl,int ll) noex {
    	int	rl = -1 ; /* return-value */
	if (sp) {
	    rl = 0 ;
	    if (ll < 0) ll = COLUMNS ;
	    if (sl < 0) sl = INT_MAX ;
	    while ((rl < sl) && (rl < ll) && *sp) {
		rl += 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (lenstrline) */


