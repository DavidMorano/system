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

	Name:
	hexval

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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<char.h>
#include	<localmisc.h>

#include	"hexval.h"


/* external subroutines */


/* forward references */


/* exported subroutines */

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


