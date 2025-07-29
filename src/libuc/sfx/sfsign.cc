/* sfsign SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the key part of a compound string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfsign

	Description:
	This subroutine will search for a numeric sign character
	(these are either '+' or '-') and skip over any white-space
	before the sign character.  It then returns the c-string
	found *after* the sign character (found above).

	Synopsis:
	int sfsign(cchar *sp,int sl,cchar **rpp,bool *bp) noex

	Arguments:
	sp		base source c-string pointer
	sl		base source c-string length
	rpp		pointer to result pointer to store found value
	bp		pointer to boolen to hold sign-result

	Returns:
	>=0		length of c-string after any found sign character

	Notes:
	This subroutine returns the string found *after* the search item.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<ischarx.h>		/* |isplusminus(3uc)| */
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfsign(cchar *sp,int sl,cchar **rpp,bool *bp) noex {
	bool		fneg = false ;
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if ((sl > 0) && isplusminus(*sp)) {
	        fneg = (*sp == '-') ;
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
	if (rpp) *rpp = sp ;
	if (bp) *bp = fneg ;
	return sl ;
}
/* end subroutine (sfsign) */


