/* modmisc%strbcpy MODULE */
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
#include	<cstring>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usys_hasx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module modmisc ;

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

local char *strbcpy_pos(char *dp,int dl,cchar *sp,int sl) noex {
	if (sl >= 0) {
	    while (dl && sl && *sp) {
		*dp++ = *sp++ ;
		sl -= 1 ;
		dl -= 1 ;
	    } /* end while */
	    *dp = eos ;
	    if ((dl == 0) && hasmore(sp,sl)) dp = nullptr ;
	} else {
	    while (dl && *sp) {
	        *dp++ = *sp++ ;
		dl -= 1 ;
	    } /* end while */
	    *dp = eos ;
	    if ((dl == 0) && *sp) dp = nullptr ;
	} /* end if */
	return dp ;
} /* end subroutine (strbcpy_pos) */

local char *strbcpy_neg(char *dp,int dl,cchar *sp,int sl) noex {
	(void) dl ;
	if (sl >= 0) {
	    while (sl-- && *sp) {
	        *dp++ = *sp++ ;
	    } /* end while */
	    *dp = eos ;
	} else {
	    dp = stpcpy(dp,sp) ;
	} /* end if */
    	return dp ;
} /* end subroutine (strbcpy_neg) */

namespace modmisc_ns {
    char *strbcpy(char *dp,int dl,cchar *sp,int sl) noex {
	char *rp = nullptr ;
    	if (dp && sp) ylikely {
	    if (dl >= 0) {
		rp = strbcpy_pos(dp,dl,sp,sl) ;
	    } else {
		rp = strbcpy_neg(dp,dl,sp,sl) ;
	    } /* end if */
	} /* end if (non-null) */
	return rp ;
    } /* end subroutine (strbcpy) */
} /* end namespace (modmisc_ns) */


