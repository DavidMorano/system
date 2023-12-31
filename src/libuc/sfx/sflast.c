/* sflast SUPPORT */
/* lang=C20 */

/* find the last <n> characters in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David D-A- Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sflast

	Description:
	This subroutine will find the last <n> characters in the
	given string.

	Synopsis:
	int sflast(cchar *sp,int sl,int n,cchar **rpp) noex

	Arguments:
	sp		given string to test
	sl		length of string to test
	n		find this (<n>) number of last characters
	rpp		pointer to receive

	Returns:
	>=0		length of found string
	<0		bad

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */


/* forward references */


/* local variables */


/* exported subroutines */

int sflast(cchar *sp,int sl,int n,cchar **rpp) noex {
	if (sl < 0) sl = strlen(sp) ;
	if ((n >= 0) && (sl > n)) {
	    sl = n ;
	    sp += (sl-n) ;
	}
	if (rpp) *rpp = sp ;
	return sl ;
}
/* end subroutine (sflast) */


