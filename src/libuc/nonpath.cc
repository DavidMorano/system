/* nonpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if the given string represent a non-path filename */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nonpath

	Description:
	I test a c-string to see if it is a floating or non-path
	filename.

	Synopsis:
	int nonpath(cchar *pp,int pl) noex

	Arguments:
	- pp	pointer to path string
	- pl	length of given path string

	Returns:
	true
	false

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<six.h>			/* |sichr(3uc)| */
#include	<strn.h>		/* |strnbrk(3uc)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"nonpath.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr cchar		nonpaths[] = "/¥§~" ;


/* exported variables */


/* exported subroutines */

int nonpath(cchar *fp,int fl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		t = 0 ; /* return-value (type-of-nonpath) */
	if (fp) ylikely {
	    if (fl < 0) fl = lenstr(fp) ;
	    rs = SR_OK ;
	    t = -1 ;
	    if ((fl > 0) && (fp[0] != '/')) {
	        if (cchar *tp ; (tp = strnbrk(fp,fl,nonpaths)) != np) {
	            if (((tp - fp) > 0) && (tp[1] != '\0')) {
			cint ch = mkchar(*tp) ;
	                t = sichr(nonpaths,-1,ch) ;
	            }
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? t : rs ;
}
/* end subroutine (nonpath) */


