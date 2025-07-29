/* sfskipwhite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove leading white space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfskipwhite

	Description:
	This subroutine will identify the non-white-space portion
	of the buffer by ignoring white space at the beginning of
	the given buffer.  No modifications to the buffer are made.

	Synopsis:
	int sfskipwhite(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		given string to test
	sl		length of string to test
	rpp		pointer to receive

	Returns:
	>=0		non-white-space string length

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* forward references */


/* forward variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfskipwhite(cchar *sp,int sl,cchar **rpp) noex {
    	if (sp) ylikely {
	    if (sl < 0) {
	        while (CHAR_ISWHITE(*sp)) sp += 1 ;
	        sl = lenstr(sp) ;
	    } else {
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	    } /* end if */
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
	if (rpp) *rpp = sp ;
	return sl ;
}
/* end subroutine (sfskipwhite) */


