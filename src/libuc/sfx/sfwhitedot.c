/* sfwhitedot SUPPORT */
/* lang=C20 */

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
	<0		error
	>=0		length of retrieved nodename

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int sfwhitedot(cchar *sp,int sl,cchar **rpp) noex {
	cchar		*tp ;
	if (sl < 0) sl = strlen(sp) ;
	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	if ((tp = strnchr(sp,sl,'.')) != NULL) {
	    sl = (tp - sp) ;
	}
	while (sl && CHAR_ISWHITE(sp[sl - 1])) {
	    sl -= 1 ;
	}
	if (rpp) {
	    *rpp = sp ;
	}
	return sl ;
}
/* end subroutine (sfwhitedot) */


