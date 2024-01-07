/* sfdequote SUPPORT */
/* lang=C20 */

/* find the dequoted sub-string of the given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfdequote

	Description:
	This subroutine strips leading and trailing double quote
	characters from the given string. It also removes leading
	and trailing white space (as defined by '|fshrink()'|.

	Synopsis:
	int sfdequote(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		string to strip
	sl		length of string to strip
	rpp		pointer to pointer to resulting stripped string

	Returns:
	-		length of resulting stripped string

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */

static int	isours(int) noex ;


/* local variables */


/* exported subroutines */

int sfdequote(cchar *sp,int sl,cchar **rpp) noex {
	if (sl < 0) sl = strlen(sp) ;
	while (sl && isours(sp[0])) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	if (sp[0] == '\0') sl = 0 ;
	if (rpp != NULL) *rpp = sp ;
	while (sl && isours(sp[sl-1])) {
	    sl -= 1 ;
	}
	return sl ;
}
/* end subroutine (sfdequote) */


/* local subroutines */

static int isours(int ch) noex {
	return (CHAR_ISWHITE(ch) || (ch == CH_QUOTE)) ;
}
/* end subroutine (isours) */


