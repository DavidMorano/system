/* ourmjd SUPPORT */
/* lang=C++20 */

/* test whether a string is composed of our MJD specifiction */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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
	0		no MJD found (not ours anyway)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<cfdec.h>
#include	<hasx.h>
#include	<char.h>
#include	<localmisc.h>

#include	"ourmjd.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int ourmjd(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (sl < 0) sl = strlen(sp) ;
	if ((sl > 1) && (CHAR_TOLC(sp[0]) == 'm')) {
	    sp += 1 ;
	    sl -= 1 ;
	    if (hasalldig(sp,sl)) {
		int	v ;
		if ((rs = cfdeci(sp,sl,&v)) >= 0) {
		    rs = v ;
		}
	    }
	} /* end if (has our 'm' marker) */
	return rs ;
}
/* end subroutine (sourmjd) */


