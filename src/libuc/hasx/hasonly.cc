/* hasonly SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasonly{x}

	Names:
	hasonlyminus
	hasonlyplusminus

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.

	Synopsis:
	bool hasonlyminus(cchar *sp,int sl) noex
	bool hasonlyplusminus(cchar *sp,int sl) noex

	Arguments:
	sp		given counted c-string pointer
	sl		given counted c-string length

	Returns:
	false
	true

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasonly.h"

#pragma		GCC dependency		"mod/libutil.ccm"

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

bool hasonlyminus(cchar *sp,int µsl) noex {
        bool            f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
            if (*sp != '\0') {
		f = true ;
                f = f && (sp[0] == '-') ;
                f = f && ((sl == 1) || (sp[1] == '\0')) ;
            }
	} /* end if (getlenstr) */
        return f ;
}
/* end subroutine (hasonlyminus) */

bool hasonlyplusminus(cchar *sp,int µsl) noex {
        bool            f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
            if (*sp != '\0') {
		f = true ;
                f = f && ((sp[0] == '+') || (sp[0] == '-')) ;
                f = f && ((sl == 1) || (sp[1] == '\0')) ;
            }
	} /* end if (getlenstr) */
        return f ;
}
/* end subroutine (hasonlyplusminus) */


