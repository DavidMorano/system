/* sileadzero SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* skip over leading zeros and leading whitespace */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Name:
	sileadzero

	Description:
	Find a string-index that skips over leading zero character
	('0') or leading whitespace (until one reaches a non-zero
	character or a non-whitespace character).  A trailing zero
	character or a trailing whitespace character is left alone.

	Synopsis:
	int sileadzero(cchar *sp,int sl) noex

	Arguments:
	sp		c-string buffer
	sl		c-string length

	Returns:
	-		length of remaining c-string after skipping leads

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sileadzero(cchar *sp,int sl) noex {
	int		i = -1 ; /* return-value */
	if (sp) ylikely {
	    cauto isx = [] (int ch) -> bool {
	        ch &= UCHAR_MAX ;
		return iszero(ch) || iswhite(ch) ;
	    } ; /* end lambda (isx) */
	    for (i = 0 ; (i < (sl - 1)) && isx(sp[0]) ; i += 1) ;
	} /* end if (non-null) */
	return i ;
}
/* end subroutine (sileadzero) */


