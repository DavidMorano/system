/* sfdomain SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to find a domain name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfdomain

	Description:
	This subroutine tries to find a domain name in the given
	counted c-string.

	Synopsis:
	int sfdomain(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		source c-string to test pointer
	sl		source c-string to test length
	rpp		pointer to result pointer to store found key

	Returns:
	>=0		length of found key string
	<0		no key was found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sfdomain.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int rmdots(cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sfdomain(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    if (int dl = rmdots(sp,sl) ; dl > 0) {
	        if (cchar *tp = strnchr(sp,dl,'.')) {
	            rl = intconv((sp + dl) - (tp + 1)) ;
		    rp = (tp + 1) ;
	        } /* end if */
	    } /* end if (rmdots) */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = rp ;
	} /* end */
	return rl ;
} /* end subroutine (sfdomain) */


/* local subroutines */

local int rmdots(cchar *sp,int sl) noex {
	while (sl && sp[0] && (sp[sl - 1] == '.')) {
	    sl -= 1 ;
	} /* end while */
	return sl ;
} /* end subroutine (rmdots) */


