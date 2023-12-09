/* siskipwhite */
/* lang=C20 */

/* subroutine to find a character in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	siskipwhite

	Description:
	This subroutine returns the number of characters skipped
	in the string due to being whitespace.

	Synopsis:
	int siskipwhite(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>0	number of characters skipped
	==0	no characters were skipped (no whitespace)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<localmisc.h>

#include	"six.h"


/* external subroutines */


/* exported subroutines */

int siskipwhite(cchar *sp,int sl) noex {
	int		i ;
	for (i = 0 ; sl-- && sp[i] ; i += 1) {
	    if (! CHAR_ISWHITE(sp[i])) break ;
	} /* end while */
	return i ;
}
/* end subroutine (siskipwhite) */


