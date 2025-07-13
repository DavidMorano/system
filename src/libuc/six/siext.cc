/* siext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if file-name (just a c-string) has an approved file-extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	siext

	Description:
        Given a file-name (a counted c-string) we determine if it has a
        file-extension of a given set.

	Synopsis:
	int siext(cchar *fp,int fl,mainv exts) noex

	Arguments:
	fp		file-name string
	fl		file-name length
	exts		arrays of strings (allowable extensions)

	Returns:
	<0		does not have an approved extension
	==0		has an approved extension, but has no base part
	>0              has an approved extension (and this is
			base-str length)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"six.h"			/* |sirbrk(3uc)| */
#include	"siext.h"

import libutil ;
import chrset ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct brkinit {
	chrset	brks ;
	constexpr brkinit() noex {
	    brks.set(CH_SLASH) ;
	    brks.set(CH_DOT) ;
	} ;
    } ; /* end struct (brkinit) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr brkinit	bi ;


/* exported variables */


/* exported subroutines */

int siext(cchar *fp,int fl) noex {
	int		si = -1 ;
	if (fp) {
	    if (fl < 0) fl = lenstr(fp) ;
	    while ((fl > 0) && (fp[fl - 1] == CH_SLASH)) {
	        fl -= 1 ;
	    }
	    if ((si = sirbrk(fp,fl,bi.brks)) >= 0) {
		if (fp[si] == CH_DOT) {
		    si += 1 ;
		} else {
		    si = -1 ;
		}
	    } /* end if (hit something) */
	} /* end if (non-null) */
	return si ;
}
/* end subroutine (siext) */


