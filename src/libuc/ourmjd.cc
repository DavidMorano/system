/* ourmjd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a counted c-string to Modified-Juluan-Day (MJD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ourmjd

	Description:
	Does the given string contain a Modified-Julian-Day (MJD)
	specification?

	Synopsis:
	int ourmjd(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	>0		MJD
	0		no MJD found
	<0		error (system-return)

	Notes:
	1. This subrouine is meant to replace the older:
		hasourmjd
	subroutine.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<cfdec.h>
#include	<hasx.h>
#include	<char.h>
#include	<localmisc.h>

#include	"ourmjd.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

int ourmjd(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    rs = SR_INVALID ;
	    if ((sl > 1) && (CHAR_TOLC(sp[0]) == 'm')) {
	        sp += 1 ;
	        sl -= 1 ;
	        if (hasalldig(sp,sl)) {
		    if (int v ; (rs = cfdeci(sp,sl,&v)) >= 0) {
		        rs = v ;
		    }
	        }
	    } /* end if (valid: has our 'm' marker) */
	} /* end if (getlenstr) */
	return rs ;
}
/* end subroutine (ourmjd) */


