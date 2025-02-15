/* ourmjd SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* test whether a string is composed of our MJD specifiction */
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

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
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


/* exported variables */


/* exported subroutines */

int ourmjd(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (sl < 0) sl = strlen(sp) ;
	if ((sl > 1) && (CHAR_TOLC(sp[0]) == 'm')) {
	    sp += 1 ;
	    sl -= 1 ;
	    if (hasalldig(sp,sl)) {
		if (int v ; (rs = cfdeci(sp,sl,&v)) >= 0) {
		    rs = v ;
		}
	    }
	} /* end if (has our 'm' marker) */
	return rs ;
}
/* end subroutine (ourmjd) */


