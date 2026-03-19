/* usupport_toxc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert charactes to a different case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	toxc

	Names:

	Description:
	Convert charactes to a different case.

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"usupport_toxc.h"

import ureserve ;			/* |char(3u)| */

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

namespace libu {
    int tolc(int ch) noex {
	return char_tolc(ch) ;
    }
    int touc(int ch) noex {
	return char_touc(ch) ;
    }
    int tofc(int ch) noex {
	return char_tofc(ch) ;
    }
} /* end namespace (libu) */


