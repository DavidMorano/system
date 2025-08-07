/* sfdequote SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the dequoted sub-string of the given string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfdequote

	Description:
	This subroutine strips leading and trailing double quote
	characters from the given string. It also removes leading
	and trailing white space (as defined by '|fshrink()'|.

	Synopsis:
	int sfdequote(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		string to strip
	sl		length of string to strip
	rpp		pointer to pointer to resulting stripped string

	Returns:
	-		length of resulting stripped string

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>		/* |CH_{xx}| */
#include	<char.h>		/* |char_iswhite(3uc)| */
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	isours(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sfdequote(cchar *sp,int sl,cchar **rpp) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    while (sl && isours(sp[0])) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if (sp[0] == '\0') sl = 0 ;
	    if (rpp) *rpp = sp ;
	    while (sl && isours(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
	return sl ;
}
/* end subroutine (sfdequote) */


/* local subroutines */

static int isours(int ch) noex {
	return (CHAR_ISWHITE(ch) || (ch == CH_QUOTE)) ;
}
/* end subroutine (isours) */


