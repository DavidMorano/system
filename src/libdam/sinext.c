/* sinext */
/* lang=C20 */

/* return index to end of next string-field */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sinext

	Description:
	Find the index to the end of the first string field within
	the given string.

	Synopsis:
	int sinext(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined

	Returns:
	>=0	index of found substring
	<0	substring not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */

#define	ISWHITE(ch)	CHAR_ISWHITE(ch)


/* external subroutines */


/* external subroutines */


/* exported subroutines */

int sinext(cchar *sp,int sl) noex {
	int		i = 0 ;
	if (sl < 0) sl = strlen(sp) ;
	while ((i < sl) && ISWHITE(sp[i])) {
	    i += 1 ;
	}
	while ((i < sl) && sp[i] && (! ISWHITE(sp[i]))) {
	    i += 1 ;
	}
	return i ;
}
/* end subroutine (sinext) */


