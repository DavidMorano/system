/* sfwhitedot SUPPORT */
/* lang=C++20 */

/* get a substring present before the first dot */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine gets the leading string before the first
	dot character (white-space cleaned up).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfwhitedot

	Description:
	This subroutine retrieves the substring before the first
	dot character.

	Synopsis:
	int sfwhitedot(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		given string
	sl		given string length
	rpp		pointer to hold result pointer

	Returns:
	>=0		length of retrieved nodename
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<string.h>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<strn.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfwhitedot(cchar *sp,int sl,cchar **rpp) noex {
	int		rl = -1 ;
	cchar		*rp = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl > 0) {
	    while (sl && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if (cchar *tp ; (tp = strnchr(sp,sl,'.')) != nullptr) {
	        sl = (tp - sp) ;
		rp = sp ;
	    }
	    while (sl && CHAR_ISWHITE(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	    if (rp) rl = sl ;
	} /* end if (non-zero positive) */
	if (rpp) *rpp = rp ;
	return rl ;
}
/* end subroutine (sfwhitedot) */


