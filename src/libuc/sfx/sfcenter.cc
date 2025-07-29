/* sfcenter SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-find the center sub-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did similar
	types of functions.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfcenter

	Description:
	This subroutine finds a cookie-key in a string.  

	Synopsis:
	int sfcenter(cchar *sp,int sl,cchar *ss,cchar **rpp) noex

	Arguments:
	sp	supplied string to test
	sl	length of supplied string to test
	ss	the delimiter
	rpp	pointer to store result "thing" pointer

	Returns:
	>=0	length of resulting key-name
	<0	no key found

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfcenter(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
	int		cl = -1 ; /* return-value */
	cchar		*cp = nullptr ;
	if (sp && ss) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl >= 2) {
	        int	sch = mkchar(ss[0]) ;
	        if (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr) {
	            sch = mkchar(ss[1]) ;
		    cp = (tp + 1) ;
	            sl -= intconv((tp + 1)-sp) ;
	            sp = (tp + 1) ;
	            if ((tp = strnchr(sp,sl,sch)) != nullptr) {
	                cl = intconv(tp - sp) ;
	            }
	        }
	    } /* end if */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (cl >= 0) ? cp : nullptr ;
	}
	return cl ;
}
/* end subroutine (sfcenter) */


