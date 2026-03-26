/* hasourmjd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the given counted c-string contains a Modified-Julian-Day (MJD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasourmjd

	Description:
	Does the given string contain a Modified-Julian-Day (MJD)
	specification?

	Synopsis:
	bool hasourmjd(cchar *sp,int sl) noex

	Arguments:
	sp		string to test
	sl		length of strin to test

	Returns:
	true		assertion succeeded
	false		assertion failed

	Notes:
	1. These (our) Modified-Julian-Dayss (MJDs) have the following
	format:
		m{dddd}
	where 'm' is the letter 'm' and {dddd} is a sring of decimal
	digits.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<char.h>		/* |CHAR_TOLC(3uc)| */
#include	<hasall.h>		/* |hasalldig(3uc)| */
#include	<localmisc.h>

#include	"hasourmjd.h"

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

bool hasourmjd(cchar *sp,int µsl) noex {
	bool		f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    if ((sl > 1) && (CHAR_TOLC(sp[0]) == 'm')) {
	        sp += 1 ;
	        sl -= 1 ;
	        f = hasalldig(sp,sl) ;
	    } /* end if (valid: has our 'm' marker) */
	} /* end if (getlenstr) */
	return f ;
}
/* end subroutine (hasourmjd) */


