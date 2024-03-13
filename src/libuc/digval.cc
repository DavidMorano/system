/* digval SUPPORT */
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

	Names:
	digval
	hexval
	decval

	Description:
	We examine a single character, supposedly a hexadecimal
	digit, and return either an error or the value of the
	symbolic hexadecimal digit.

	Synopsis:
	int hexval(int ch) noex

	Arguments:
	ch		character to evaluate

	Outputs:
	>=0		value of symbolic hexadecimal digit
	<0		error (system-return)

	Notes:
	subroutine	domain
	----------------------------------
	digval		base-64 (original + checked)
	hexval		base-16 (checked)
	decval		base-10 (checked)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<char.h>

#include	"digval.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* exported subroutines */

int digval(int ch) noex {
	int		v = char_toval(ch) ;
	if (v == UCHAR_MAX) v = SR_DOM ;
	return v ;
}
/* end subroutine (digval) */

int hexval(int ch) noex {
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
/* end subroutine (hexval) */

int decval(int ch) noex {
	int		v = SR_DOM ;
	if ((ch >= '0') && (ch <= '9')) {
	    v = (ch - '0') ;
	}
	return v ;
}
/* end subroutine (decval) */


