/* sichr */
/* lang=C20 */

/* subroutine to find the index of a character in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sichr

	Description:
	This subroutine searches for a character within a given
	string and returns the index to that character (if it is
	found).  It returns (-1) if the character does not exist
	within the given string.

	Synopsis:
	int sichr(cchar *sp,int sl,int sch) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	sch	character to search for

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>
#include	<ascii.h>
#include	<toxc.h>

#include	"six.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int sichr(cchar *sp,int sl,int sch) noex {
	int		i = 0 ;
	bool		f = false ;
	sch &= UCHAR_MAX ;
	for (i = 0 ; sl-- && sp[i] ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = (ch == sch) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (sichr) */


