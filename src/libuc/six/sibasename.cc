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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"six.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

int sibasename(cchar *sp,int µsl) noex {
	int		si = -1 ; /* return-value */
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    while ((sl > 0) && (sp[sl - 1] == '/'))  {
	        sl -= 1 ;
	    }
	    for (si = sl ; si > 0 ; si -= 1) {
	        if (sp[si - 1] == '/') break ;
	    }
	} /* end if (getlenstr) */
	return si ;
} /* end subroutine (sibasename) */


