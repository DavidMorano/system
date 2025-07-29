/* digval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the value of a single hexadecimal digit */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was adapted from assembly.  The original
	assembly goes wa...ay back.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	digval

	Names:
	digval
	digvalbin
	digvaloct
	digvaldec
	digvalhex

	Description:
	We examine a single character, supposedly a hexadecimal
	digit, and return either an error or the value of the
	symbolic hexadecimal digit.

	Synopsis:
	int digval(int ch) noex
	int digvalbin(int ch) noex
	int digvaloct(int ch) noex
	int digvaldec(int ch) noex
	int digvalhex(int ch) noex

	Arguments:
	ch		character to evaluate

	Outputs:
	>=0		value of symbolic digit
	<0		error (system-return)

	Notes:
	subroutine	domain
	----------------------------------
	digval		base-64 (original + checked)
	digvalbin	base-2 (checked)
	digvaloct	base-8 (checked)
	digvaldec	base-10 (checked)
	digvalhex	base-16 (checked)

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
#include	<char.h>		/* |char_tolc(3uc)| */
#include	<localmisc.h>

#include	"digval.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int digval(int ch) noex {
	int		v = char_toval(ch) ;
	if (v == UCHAR_MAX) v = SR_DOM ;
	return v ;
}
/* end subroutine (digval) */

int digvalbin(int ch) noex {
	int		v = SR_DOM ;
	if ((ch >= '0') && (ch <= '1')) {
	    v = (ch - '0') ;
	}
	return v ;
}
/* end subroutine (digvalbin) */

int digvaloct(int ch) noex {
	int		v = SR_DOM ;
	if ((ch >= '0') && (ch <= '7')) {
	    v = (ch - '0') ;
	}
	return v ;
}
/* end subroutine (digvaloct) */

int digvaldec(int ch) noex {
	int		v = SR_DOM ;
	if ((ch >= '0') && (ch <= '9')) {
	    v = (ch - '0') ;
	}
	return v ;
}
/* end subroutine (digvaldec) */

int digvalhex(int ch) noex {
	int		v = SR_DOM ;
	if ((ch >= '0') && (ch <= '9')) {
	    v = (ch - '0') ;
	} else {
	    cint	lch = CHAR_TOLC(ch) ;
	    if ((lch >= 'a') && (lch <= 'f')) {
	        v = ((lch - 'a') + 10) ;
	    }
	}
	return v ;
}
/* end subroutine (digvalhex) */


