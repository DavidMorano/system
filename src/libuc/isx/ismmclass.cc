/* ismmclass SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* test a character for a property */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ismmclass_7bit
	ismmclass_8bit
	ismmclass_binary

	Description:
	These subroutines test whether a given character falls within
	a certain MIME content-encoding class.  Yhe classes supported
	are:
		7bit
		8bit
		binary

	Synopsis:
	bool ismmclass_7bit(int ch) noex
	bool ismmclass_8bit(int ch) noex
	bool ismmclass_binary(int ch) noex

	Arguments:
	ch		character to test

	Returns:
	false
	true

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"ischarx.h"		/* |isprintlatin(3uc)| */
#include	"ismmclass.h"


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

/* is it 7-bit text (no controls or other weirdo) */
bool ismmclass_7bit(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if (ch < 0x80) {
	    f = f || isprintlatin(ch) ;
	    f = f || (ch == '\n') ;
	    f = f || (ch == '\r') ;
	    f = f || (ch == CH_TAB) ;
	    f = f || (ch == CH_SP) ;
	}
	return f ;
}
/* end subroutine (ismmclass_7bit) */

/* does it *require* 8-bit but *only* 8-bit */
bool ismmclass_8bit(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if (ch >= 0x80) {
	    f = ((ch & 0x7f) >= 0x20) ;
	}
	return f ;
}
/* end subroutine (ismmclass_8bit) */

/* does it *require* binary */
bool ismmclass_binary(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if (((ch & 0x7f) < 0x20) || (ch == CH_DEL)) {
	    f = true ;
	    f = f && (ch != CH_TAB) ;
	    f = f && (ch != '\n') ;
	    f = f && (ch != '\r') ;
	}
	return f ;
}
/* end subroutine (ismmclass_binary) */


