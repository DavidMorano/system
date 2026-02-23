/* hasbasename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a counted c-string contain a C++ module-name? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasbasename

	Description:
	Determine if if the specified counted c-string (interpreted
	as a file-path) has a (reasonably) valid C++ basename.

	Synopsis:
	int hasbasename(cchar *sp,int sl,cchar *bname) noex

	Arguments:
	sp		c-string to test pointer
	sl		c-string to test length
	bname		base-name

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
#include	<usupport.h>		/* |strwcmp(3u)| */
#include	<strwcmp.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |isalphalatin(3u)| */
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasbasename.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* |sfbasename(3u)| */

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

bool hasbasename(cchar *sp,int µsl,cchar *bname) noex {
        bool            f = false ;
	if (int sl ; bname && ((sl = getlenstr(sp,µsl)) > 0)) {
	    if (bname[0]) {
		cchar *cp ;
		if (int cl ; (cl = sfbasename(sp,sl,&cp)) > 0) {
		    f = (strwcmp(bname,cp,cl) == 0) ;
                } /* end if (sfbasename) */
	    } /* end if (valid) */
	} /* end if (getlenstr) */
        return f ;
} /* end subroutine (hasbasename) */


