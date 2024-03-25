/* sialpha SUPPORT */
/* lang=C++20 */

/* subroutine to find the index of an alpha character in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sialpha

	Description:
	This subroutine searchs for an alpha character within a
	given string and returns the index to that character (if
	it is found).  It returns -1 if the character does not exist
	within the given string.

	Synopsis:
	int sialpha(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int sialpha(cchar *sp,int sl) noex {
	int		i = 0 ;
	bool		f = false ;
	for (i = 0 ; sl && sp[i] ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = isalphalatin(ch) ;
	    if (f) break ;
	    sl -= 1 ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (sialpha) */


