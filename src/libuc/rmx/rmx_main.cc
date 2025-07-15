/* rmx_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove characters from the end of the given c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2000 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	rmeol
	rmochr
	rmrchr
	rmext
	rmtrailchr
	rmcomment

	Description:
	Return the length of the given string without any EOL
	(End-Of-Line) characters included. Any EOL characters are
	only considered starting from the end of the counted string.

	Synopsis:
	int rmeol(cchar *sp,int sl) noex
	int rmochr(cchar *sp,int sl,int sch) noex
	int rmrchr(cchar *sp,int sl,int sch) noex
	int rmext(cchar *sp,int sl) noex
	int rmtrailchr(cchar *sp,int sl,int sch) noex
	int rmcomment(cchar *sp,int sl) noex

	Arguments:
	sp		pinter to constant string to consider
	sl		length of supplied string
	sch		character to search for

	Returns:
	-		length of string without removed ending

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>		/* |strn{x}chr(3uc)| */
#include	<six.h>			/* |siext(3uc)| */
#include	<ischarx.h>		/* |iseol(3uc)| */
#include	<localmisc.h>

#include	"rmx.h"

import libutil ;

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

int rmeol(cchar *sp,int sl) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    while (sl && iseol(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	}
	return sl ;
}
/* end subroutine (rmeol) */

int rmochr(cchar *sp,int sl,int ch) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *tp ; (tp = strnochr(sp,sl,ch)) != nullptr) {
	        sl = intconv(tp - sp) ;
	    }
	} else {
	    sl = -1 ;
	} /* end if */
	return sl ;
}
/* end subroutine (rmochr) */

int rmrchr(cchar *sp,int sl,int ch) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *tp ; (tp = strnrchr(sp,sl,ch)) != nullptr) {
	        sl = intconv(tp - sp) ;
	    }
	} else {
	    sl = -1 ;
	} /* end if */
	return sl ;
}
/* end subroutine (rmrchr) */

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

int rmtrailchr(cchar *sp,int sl,int sch) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 1) && (sp[sl - 1] == char(sch))) {
	       sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	}
	return sl ;
}
/* end subroutine (rmtrailchr) */

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


