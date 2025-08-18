/* hasvarprefix SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* has a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasvarprefix

	Description:
	This subroutine tests whether the file-name has the var-path
	prefix on it.  A so-called "var-path" prefix looks like
	either one of the following:
		%<string>/<comething>/<...>
		/%<string>/<something>/...

	Synopsis:
	bool hasvarprefix(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		assertion fails
	true		assertion succeeds

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasx.h"

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

bool hasvarprefix(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    cint	chx_ec = '%' ;
	    f = f || (sl && (sp[0] == chx_ec)) ;
	    if (! f) {
		f = true ;
	        f = f && ((sl < 0) || (sl > 1)) ;
		f = f && (sp[0] == '/') && (sp[1] == chx_ec) ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasvarprefix) */


