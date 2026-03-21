/* rmx_misc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove characters from the end of the given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	rmext
	rmcomment

	Description:
	Search the given counted c-string, from its end, for a
	character (or set of characters) that meet the given criteria
	and return the remaining length of the up until that point.

	Synopsis:
	int rmext(cchar *sp,int sl) noex
	int rmcomment(cchar *sp,int sl) noex

	Arguments:
	sp		counted c-string pinter
	sl		counted c-string length

	Returns:
	>=0		index of found criteria
	<0		critera was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>		/* |strn{x}chr(3uc)| */
#include	<six.h>			/* |siext(3uc)| */
#include	<ischarx.h>		/* |iseol(3uc)| */
#include	<localmisc.h>

#include	"rmx.h"

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

int rmext(cchar *sp,int sl) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
    	    if (cint si = siext(sp,sl) ; si >= 0) {
	        sl = si ;
	    }
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
    	return sl ;
}
/* end subroutine (rmext) */

int rmcomment(cchar *lp,int ll) noex {
	int		rl = -1 ;
	if (lp) ylikely {
	    if (ll < 0) ll = lenstr(lp) ;
	    if ((rl = rmochr(lp,ll,'#')) == ll) {
	       rl = rmeol(lp,ll) ;
	    }
	} /* end if (non-null) */
	return rl ;
}
/* end subroutine (rmcomment) */


