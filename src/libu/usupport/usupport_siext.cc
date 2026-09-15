/* usupport_siext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find string-index up to the beginning of a file-name extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sbu::iext

	Description:
	Given a file-name (a counted c-string) I determine if it
	has a file-extension.  If it has a file-extension, I return
	the length (character count) of the origial string up to
	the start of the file-extension (after the dot character).
	For no file-extension, I return -1.

	Synopsis:
	int libu::siext(cchar *fp,int fl) noex

	Arguments:
	fp		file-name string
	fl		file-name length

	Returns:
	>=0		leading length up to found file-extension
	<0              no file extension found

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_strnxbrk.hh"
#include	"usupport_siext.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::strnrbrk ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int siext(cchar *fp,int µfl) noex {
	int		si = -1 ;
	if (int fl ; (fl = getlenstr(fp,µfl)) >= 0) {
	    while ((fl > 0) && (fp[fl - 1] == CH_SLASH)) {
	        fl -= 1 ;
	    } /* end */
	    if (cc *tp = strnrbrk(fp,fl,"/.")) {
		if (*tp == CH_DOT) {
		    si = intconv((tp + 1) - fp) ;
		}
	    } /* end if (hit something) */
	} /* end if (getlenstr) */
	return si ;
    } /* end subroutine (siext) */
} /* end namespce (libu) */


