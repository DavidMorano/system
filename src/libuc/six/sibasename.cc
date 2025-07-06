/* sibasename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the base file name out of a file-path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name
	sibasename

	Description:
	This routine returns the index within the given string to
	the start of the base-name portion of the given path string.

	Synopsis:
	int sibasename(cchar *sp,int sl) noex

	Arguments:
	sp	given path string
	sl	length of given path string (can be -1)

	Returns:
	>=	index of found string
	<0	caller-usage-error (should not normally happen)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"

import libutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sibasename(cchar *sp,int sl) noex {
	int		si = -1 ; /* return-value */
	if (sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 0) && (sp[sl - 1] == '/'))  {
	        sl -= 1 ;
	    }
	    for (si = sl ; si > 0 ; si -= 1) {
	        if (sp[si - 1] == '/') break ;
	    }
	} /* end if (non-null) */
	return si ;
}
/* end subroutine (sibasename) */


