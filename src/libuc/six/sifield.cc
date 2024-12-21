/* sifield SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutine to find the index of a character in a given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sifield

	Description:
	This subroutine searchs for a character in the given string
	and returns the index to either the search-character or to
	the end of the given string.

	Synopsis:
	int sifield(cchar *sp,int sl,int sch) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined
	sch	character to search for

	Returns:
	>=0	field length

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int sifield(cchar *sp,int sl,int sch) noex {
	int		i = 0 ; /* used afterwads */
	bool		f = false ;
	sch &= UCHAR_MAX ;
	for (i = 0 ; sl-- && sp[i] ; i += 1) {
	    cint	ch = (sp[i] & UCHAR_MAX) ;
	    f = (ch == sch) ;
	    if (f) break ;
	} /* end for */
	return i ;
}
/* end subroutine (sifield) */


