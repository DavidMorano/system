/* usupport_hasx SUPPORT */
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
	hasnotdots

	Description:
	This subroutine determines if the given string has neither of the
	following:
	+ one dot character
	+ two dot characters

	Synopsis:
	bool hasNotDots(cchar *sp,int sl) noex

	Arguments:
	sp		pointer to given string
	sl		length of given string

	Returns:
	false		string has the standard dot-dirs
	true		string does not have the standard dot-dirs

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"usupport_hasx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

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

namespace libu {
    bool hasnotdots(cchar *sp,int µsl) noex {
	bool		f = true ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    if (sp[0] == '.') {
	        if (sl <= 2) {
	            f = (sl != 1) ;
		    f = f || ((sl == 2) && (sp[1] != '.')) ;
	        }
	    } /* end if (had a leading dot) */
	} /* end if (getlenstrmagic) */
	return f ;
    } /* end subroutine (hasnotdots) */
} /* end namespace (libu) */


