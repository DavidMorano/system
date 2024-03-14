/* endian SUPPORT */
/* lang=C++20 */

/* machine endian management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides machine endian managemnt.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<bit>			/* |endian(3c++)| */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"endian.h"


/* local defines */


/* imported namespaces */

using std::endian ;			/* enumeration */


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
	if constexpr (endian::native == endian::little) {
	    endianval = 0 ;
	    endianstr = "0" ;
	} else if constexpr (endian::native == endian::big) {
	    endianval = 1 ;
	    endianstr = "1" ;
	} else {
	    endianval = 2 ;
	    endianstr = "2" ;
	} /* end if-constexpr (endian) */
}
/* end method (endianhelp::endianhelp) */


