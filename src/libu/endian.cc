/* endian SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* machine endian management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides machine endian managemnt.  Basically,
	this code just sets two global variables with an indication
	of what the machine endianess is.  The two varaibles are
	(below):

 	+ endianval
	+ endianstr

	For little endian machines:
		endianval == 0
		anddianstr == "0"
	For big endian machines:
		endianval == 1
		anddianstr == "1"

	To the programmer, these variables are available (for
	somewhat historical reasons) as the following preprocessor
	defined macros:
		ENDIAN (endianval)
		ENDIANSTR (endianstr)

	Both the (global) variables and preprocessor macros are
	avilable for both the C language and the C++ language.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bit>			/* |endian(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"endian.h"


/* local defines */


/* imported namespaces */

using std::endian ;			/* enumeration */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct endianhelp {
	endianhelp() noex ;
    } ; /* end struct (endianhelp) */
}


/* forward references */


/* local variables */

static endianhelp	helper ;


/* exported variables */

int	endianval ;

cchar	*endianstr ;


/* exported subroutines */


/* local subroutines */

endianhelp::endianhelp() noex {
	if_constexpr (endian::native == endian::little) {
	    endianval = 0 ;
	    endianstr = "0" ;
	} else if_constexpr (endian::native == endian::big) {
	    endianval = 1 ;
	    endianstr = "1" ;
	} else {
	    endianval = 2 ;
	    endianstr = "2" ;
	} /* end if_constexpr (endian) */
}
/* end ctor (endianhelp::endianhelp) */


