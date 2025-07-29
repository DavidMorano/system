/* sinext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return index to end of next string-field */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sinext

	Description:
	Find the index to the end of the first string field within
	the given string.

	Synopsis:
	int sinext(cchar *sp,int sl) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined

	Returns:
	>=0	index of beginning of next field
	<0	substring not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"

import libutil ;

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

int sinext(cchar *sp,int sl) noex {
	int		i = -1 ; /* return-value */
	if (sp) ylikely {
	    i = 0 ; 
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((i < sl) && iswht(sp[i])) {
	        i += 1 ;
	    }
	    while ((i < sl) && sp[i] && (! iswht(sp[i]))) {
	        i += 1 ;
	    }
	} /* end if (non-null) */
	return i ;
}
/* end subroutine (sinext) */


