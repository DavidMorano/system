/* sfbracketval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the string-value within brackets */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
        This code module was completely rewritten to replace any original
        garbage that was here before, if any.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfbracketval

	Description:
	We find a sub-string within brackets.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>
#include	<char.h>
#include	<localmisc.h>

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
    	if (sp) ylikely {
	    if (cc *tp ; (tp = strnchr(sp,sl,'<')) != nullptr) {
	        tp += 1 ;
	        sl = intconv((sp + sl) - tp) ;
	        if (cc *t2p ; (t2p = strnchr(tp,sl,'>')) != nullptr) {
	            sl = intconv(t2p - tp) ;
	        }
	        sp = tp ;
	    } /* end if */
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    while ((sl > 0) && CHAR_ISWHITE(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
	if (rpp) *rpp = sp ;
	return sl ;
}
/* end subroutine (sfbracketval) */


