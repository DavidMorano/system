/* sfbracketval SUPPORT */
/* lang=C20 */

/* find the string-value within brackets */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
        This code module was completely rewritten to replace any original
        garbage that was here before, if any.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We find a sub-string within brackets.

*******************************************************************************/

#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<strn.h>
#include	<char.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfbracketval(cchar *sp,int sl,cchar **rpp) noex {
	cchar		*cp2 ;
	cchar		*cp3 ;
	if ((cp2 = strnchr(sp,sl,'<')) != nullptr) {
	    cp2 += 1 ;
	    sl = (sp + sl) - cp2 ;
	    if ((cp3 = strnchr(cp2,sl,'>')) != nullptr) {
	        sl = cp3 - cp2 ;
	    }
	    sp = cp2 ;
	} /* end if */
	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	while ((sl > 0) && CHAR_ISWHITE(sp[sl - 1])) {
	    sl -= 1 ;
	}
	if (rpp) {
	    *rpp = sp ;
	}
	return sl ;
}
/* end subroutine (sfbracketval) */


