/* sfsign SUPPORT */
/* lang=C++20 */

/* get the key part of a compound string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfsign

	Description:
	This subroutine will search for numeric sign character
	(these are either '+' or '-') and skip over any white-space
	before the sign character.

	Synopsis:
	int sfsign(bool *bp,cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	bp		pointer to boolen to hold sign-result
	sp		base string pointer
	sl		base string length
	rpp		pointer to result pointer to store found value

	Returns:
	>=0		length of c-string after any found sign character

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<ischarx.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfsign(bool *bp,cchar *sp,int sl,cchar **rpp) noex {
	bool		fn = false ;
	if (sl < 0) sl = strlen(sp) ;
	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	if ((sl > 0) && isplusminus(*sp)) {
	    fn = (*sp == '-') ;
	    sp += 1 ;
	    sl -= 1 ;
	}
	*rpp = sp ;
	*bp = fn ;
	return sl ;
}
/* end subroutine (sfsign) */


