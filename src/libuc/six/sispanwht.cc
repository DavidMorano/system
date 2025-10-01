/* sispanwht SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* count leading white-space characters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sispanwht

	Description:
	This subroutine returns the number of leading white-space
	characters skipped in the given c-string.

	Synopsis:
	int sispanwht(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on

	Returns:
	>0	number of characters skipped
	==0	no characters were skipped (no whitespace)
	<0	error (null-pointer)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>
#include	<localmisc.h>

#include	"sispanwht.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cauto		iswht = char_iswhite ;


/* exported variables */


/* exported subroutines */

int sispanwht(cchar *sp,int sl) noex {
	int		i = -1 ; /* used afterwards */
	if (sp) ylikely {
	    for (i = 0 ; sl-- && sp[i] ; i += 1) {
	        if (! iswht(sp[i])) break ;
	    } /* end while */
	} /* end if (non-null) */
	return i ;
}
/* end subroutine (sispanwht) */


