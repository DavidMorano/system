/* rmleadzero SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove leading white-space and zero characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Name:
	rmleadzero

	Description:
	Remove leading white-space and zero characters from a c-string.

	Synopsis:
	int rmleadzero(cchar *sp,int sl) noex

	Arguments:
	sp		counted c-string pinter
	sl		counted c-string length

	Returns:
	>=0		length of remaining c-string after leading removal
	<0		error (NULL c-string given)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ischarx.h>		/* |iszero(3uc)| + |iswht(3uc)| */
#include	<localmisc.h>

#include	"rmleadzero.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool isx(int ch) noex {
	ch &= UCHAR_MAX ;
	return iszero(ch) || iswht(ch) ;
} /* end subroutine (isx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int rmleadzero(cchar *sp,int µsl) noex {
	int		nsl = -1 ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    nsl = sl ;
	    for (int i = 0 ; (i < (sl - 1)) && isx(sp[i]) ; i += 1) {
	        nsl -= 1 ;
	    } /* end for */
	} /* end if (getlenstr) */
	return nsl ;
}
/* end subroutine (rmleadzero) */


