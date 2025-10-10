/* sixterm SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the index in a c-string up to a terminator character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sioterm
	sirterm

	Description:
	This subroutine searches for a terminator character within
	a given c-string and returns the index to that character
	(if it is found).  It returns (-1) if the character does
	not exist within the given string.

	Synopsis:
	int sioterm(cchar *sp,int sl,cchar *terms) noex
	int sirterm(cchar *sp,int sl,cchar *terms) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
	terms	bit-block containing terminator characters

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
#include	<baops.h>		/* |batst(3u)| */
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sixterm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sioterm(cchar *sp,int sl,cchar *terms) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	if (sp && terms) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    for (i = 0 ; (i < sl) && sp[i] ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        f = batst(terms,ch) ;
	        if (f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
}
/* end subroutine (sioterm) */

int sirterm(cchar *sp,int sl,cchar *terms) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	if (sp && terms) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    for (i = (sl - 1) ; (i >= 0) ; i += 1) {
	        cint	ch = mkchar(sp[i]) ;
	        f = batst(terms,ch) ;
	        if (f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
}
/* end subroutine (sirterm) */


