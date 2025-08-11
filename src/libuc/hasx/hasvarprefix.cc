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

  	Group:
	hasx

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.


	Name:
	hasvarprefix

	Description:
	This subroutine tests whether the file-name has the var-path
	prefix on it.  A so-called "var-path" prefix looks like
	either one of the following:

	%<string>/<comething>/<...>
	/%<string>/<something>/...

	Synopsis:
	bool hasvarprefix(cchar *,int) noex

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |strcmp(3c)| + |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<mkchar.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasx.h"

import libutil ;			/* |lenstr(3u)| */
import ulibvals ;			/* |ulibval(3u)| */

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


