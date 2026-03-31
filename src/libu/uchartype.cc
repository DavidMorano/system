/* uchartype SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* charcter classification tests */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* order first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"uchartype.h"

#pragma		GCC dependency		"mod/ureserve.ccm"

import ureserve ;			/* |is{x}(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool isdigex(int ch) noex attrpure {
    	return isdigexlatin(ch) ;
} /* end subroutine (isdigex) */

bool iswhite(int ch) noex attrpure {
    	return iswhitelatin(ch) ;
} /* end subroutine (iswhite) */


