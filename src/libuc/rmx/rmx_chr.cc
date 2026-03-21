/* rmx_chr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove characters from counted c-string before or after a given character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	rmochr
	rmrchr

	Description:
	Search the given counted c-string, either forwards or 
	backwards, for the given character and return the 
	index of that found character, or -1 if not found.

	Synopsis:
	int rmochr(cchar *sp,int sl,int sch) noex
	int rmrchr(cchar *sp,int sl,int sch) noex

	Arguments:
	sp		counted c-string pinter
	sl		counted c-string length
	sch		character to search for

	Returns:
	>=0		index of found character
	<0		chracter not found

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

extern "C" {
    typedef char *(*strnx_f)(cchar *,int,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int rmxchr(strnx_f sx,cchar *sp,int sl,int ch) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *tp ; (tp = sx(sp,sl,ch)) != nullptr) {
	        sl = intconv(tp - sp) ;
	    }
	} else {
	    sl = -1 ;
	} /* end if */
	return sl ;
} /* end subroutine (rmxchr) */


/* local variables */


/* exported variables */


/* exported subroutines */

int rmochr(cchar *sp,int sl,int ch) noex {
    	return rmxchr(strnochr,sp,sl,ch) ;
} /* end subroutine (rmochr) */

int rmrchr(cchar *sp,int sl,int ch) noex {
    	return rmxchr(strnrchr,sp,sl,ch) ;
} /* end subroutine (rmrchr) */


