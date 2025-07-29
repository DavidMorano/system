/* sixchr SUPPORT */
/* charset=ISO8859-1 */
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
	siochr
	sirchr

	Description:
	This subroutine searches for a character within a given
	string and returns the index to that character (if it is
	found).  It returns (-1) if the character does not exist
	within the given string.

	Synopsis:
	int siochr(cchar *sp,int sl,int sch) noex
	int sirchr(cchar *sp,int sl,int sch) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	sch	character to search for

	Returns:
	>=0	index of search character in the given string
	<0	the search character was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sixchr.h"

import libutil ;

/* local defines */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int siochr(cchar *sp,int sl,int sch) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	sch &= UCHAR_MAX ;
	if (sp) ylikely {
	    for (i = 0 ; sl-- && sp[i] ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        f = (ch == sch) ;
	        if (f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
}
/* end subroutine (siochr) */

int sirchr(cchar *sp,int sl,int sch) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	sch &= UCHAR_MAX ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    for (i = (sl - 1) ; i >= 0 ; i -= 1) {
	        cint	ch = mkchar(sp[i]) ;
	        f = (ch == sch) ;
	        if (f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
}
/* end subroutine (sirchr) */


