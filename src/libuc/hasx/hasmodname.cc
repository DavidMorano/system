/* hasmodname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* has a counted c-string contain a C+ module-name? */
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
	This subroutine checks if a specified c-string has a (reasonably)
	valid C++ module name.

	Synopsis:
	int hasmodname(cchar *sp,int sl) noex

	Arguments:
	sp		c-string to test pointer
	sl		c-string to test length

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
#include	<ascii.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |salphalatin(3u)| */
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasmodname.h"

import libutil ;			/* |lenstr(3u)| */

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

bool hasmodname(cchar *mp,int ml) noex {
        bool            f = false ;
	if (mp) ylikely {
            if (int ch ; (ml > 0) && (ch = mkchar(*mp) , isalphalatin(ch))) {
                cint	ch_d = CH_DOT ;
                cint	ch_u = CH_UNDER ;
                while (ml-- && *mp) {
                    ch = mkchar(*mp++) ;
                    f = isalnumlatin(ch) || (ch == ch_d) || (ch == ch_u) ;
                    if (! f) break ;
                } /* end while */
            } /* end if (correct leading character) */
	} /* end if (non-null) */
        return f ;
} /* end subroutine (hasmodname) */


