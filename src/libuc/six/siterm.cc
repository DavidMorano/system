/* siterm SUPPORT */
/* lang=C++20 */

/* find the index in a c-string up to a terminator character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	siterm

	Description:
	This subroutine searches for a terminator character within
	a given c-string and returns the index to that character
	(if it is found).  It returns (-1) if the character does
	not exist within the given string.

	Synopsis:
	int siterm(cchar *sp,int sl,cchar *terms) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	terms	bit-block containing terminator characters

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<mkchar.h>
#include	<toxc.h>
#include	<baops.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int siterm(cchar *sp,int sl,cchar *terms) noex {
	int		i = 0 ;
	bool		f = false ;
	if (sl < 0) sl = strlen(sp) ;
	for (i = 0 ; (i < sl) && sp[i] ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    f = batst(terms,ch) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (siterm) */


