/* rmeol */
/* lang=C20 */

/* remove EOL (End-Of-Line) characters from the given string */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	rmeol

	Description:
	Return the length of the given string without any EOL
	(End-Of-Line) characters included. Any EOL characters are
	only considered starting from the end of the counted string.

	Synopsis:
	int rmeol(cchar *sp,int sl) noex

	Arguments:
	sp		pinter to constant string to consider
	sl		length of supplied string

	Returns:
	-		length of string without any EOL characters


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<ischarx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported subroutines */

int rmeol(cchar *sp,int sl) noex {
	if (sl < 0) sl = strlen(sp) ;
	while (sl && iseol(sp[sl-1])) sl -= 1 ;
	return sl ;
}
/* end subroutine (rmeol) */


