/* siext SUPPORT */
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
	siext

	Description:
	Given a file-name (a counted c-string) I determine if it
	has a file-extension.  If it has a file-extension, I return
	the length (character count) of the origial string up to
	the startof the file-extension (after the dot character).
	For no file-extension, I return -1.

	Synopsis:
	int siext(cchar *fp,int fl,mainv exts) noex

	Arguments:
	fp		file-name string
	fl		file-name length
	exts		arrays of strings (allowable extensions)

	Returns:
	>=0		leading length up to found file-extension
	<0              no file extension found

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

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"mod/chrset.ccm"

import libutil ;
import chrset ;

/* local defines */


/* external subroutines */

extern "C++" {
    int		siobrk(cchar *,int,const chrset &) noex ;
    int		sirbrk(cchar *,int,const chrset &) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct brkinit {
	chrset		brks ;
	brkinit() noex {
	    brks.set(CH_SLASH) ;
	    brks.set(CH_DOT) ;
	} ; /* end ctor */
    } ; /* end struct (brkinit) */
} /* end namespace */


/* forward references */


/* local variables */

local brkinit		bi ;


/* exported variables */


/* exported subroutines */

int siext(cchar *fp,int fl) noex {
	int		si = -1 ;
	if (fp) ylikely {
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


