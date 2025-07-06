/* sialnum SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to find the index of an alpha-numeric character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sialnum

	Description:
	This subroutine searchs for an alpha-numeric character
	within a given string and returns the index to that character
	(if it is found).  It returns -1 if the character does not
	exist within the given string.

	Synopsis:
	int sialnum(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int sialnum(cchar *sp,int sl) noex {
	int		i ; /* return-value */
	bool		f = false ;
	for (i = 0 ; sl && sp[i] ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = isalnumlatin(ch) ;
	    if (f) break ;
	    sl -= 1 ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (sialnum) */


