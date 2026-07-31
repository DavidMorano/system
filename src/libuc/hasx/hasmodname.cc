/* hasmodname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the counted c-string contain a C++ module-name? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasmodname

	Description:
	This subroutine checks if a specified c-string has a
	(reasonably) valid C++ module name.

	Synopsis:
	int hasmodname(cchar *sp,int sl) noex

	Arguments:
	sp		c-string to test pointer
	sl		c-string to test length

	Returns:
	false		assertion fails
	true		assertion succeeds

	Notes:
	1. A module name consists of:
	+ a leading alpha character
	+ all remaining characters are alph-numeric or underscore
	  or a dot

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"hasmodname.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* |ix{x}(3u)| */

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

bool hasmodname(cchar *sp,int µsl) noex {
        bool            f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
            if (int ch = mkchar(*sp) ; isalphalatin(ch)) {
                cint	ch_d = CH_DOT ;
                cint	ch_u = CH_UNDER ;
                while (sl-- && *sp) {
                    ch = mkchar(*sp++) ;
                    f = isalnumlatin(ch) || (ch == ch_d) || (ch == ch_u) ;
                    if (! f) break ;
                } /* end while */
            } /* end if (correct leading character) */
	} /* end if (getlenstr) */
        return f ;
} /* end subroutine (hasmodname) */


