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
	hasbasestem

	Description:
	Determine if if the specified counted c-string (interpreted
	as a file-path) has a (reasonably) valid C++ basename.

	Synopsis:
	int hasbasename(cchar *sp,int sl,cchar *bname) noex
	int hasbasestem(cchar *sp,int sl,cchar *sname) noex

	Arguments:
	sp		c-string to test pointer
	sl		c-string to test length
	bname		base-name
	sname		stem-name

	Returns:
	false		assertion fails
	true		assertion succeeds

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<strwcmp.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC |isalphalatin(3u)| */
#include	<localmisc.h>		/* LIBU |UC(3u)| */

#include	"hasbasename.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* |sfbasename(3u)| */

/* local defines */


/* imported namespaces */

using libu::strnrchr ;			/* subroutine */


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
	    if (cchar *cp ; bname[0]) {
		if (int cl ; (cl = sfbasename(sp,sl,&cp)) > 0) {
		    f = (strwcmp(bname,cp,cl) == 0) ;
                } /* end if (sfbasename) */
	    } /* end if (valid) */
	} /* end if (getlenstr) */
        return f ;
} /* end subroutine (hasbasename) */

bool hasbasestem(cchar *sp,int µsl,cchar *sname) noex {
        bool            f = false ;
	if (int sl ; sname && ((sl = getlenstr(sp,µsl)) > 0)) {
	    if (cchar *cp ; sname[0]) {
		if (int cl ; (cl = sfbasename(sp,sl,&cp)) > 0) {
		    if (cchar *tp = strnrchr(cp,cl,'.') ; tp) {
			cint tl = intconv(tp - cp) ;
		        f = (strwcmp(sname,cp,tl) == 0) ;
		    }
                } /* end if (sfbasename) */
	    } /* end if (valid) */
	} /* end if (getlenstr) */
        return f ;
} /* end subroutine (hasbasestem) */


