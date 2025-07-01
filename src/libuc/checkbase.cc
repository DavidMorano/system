/* checkbase SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* test whether a string is composed of all characters of a given base */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	checkbase

	Description:
	This subroutine is mostly used as a utility function to
	determine (verify) that all of the characters in a counted
	string are consistent to be be used as digits within the
	supplied numeric base.

	Synopsis:
	int checkbase(cchar *sp,int sl,int b) noex

	Arguments:
	sp		string to test
	sl		length of strin to test
	b		base to check against

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>		/* |xstrlen(3u)| + ret-status */
#include	<hasx.h>
#include	<localmisc.h>

#include	"checkbase.h"

import ulibvals ;			/* |ulibval(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cint	maxbase = ulibval.maxbase ;


/* exported variables */


/* exported subroutines */

int checkbase(cchar *sp,int sl,int b) noex {
	int		rs = SR_INVALID ;
	if ((b >= 2) && (b <= maxbase)) {
	    rs = (hasallbase(sp,sl,b)) ? SR_OK : SR_DOM ;
	}
	return rs ;
}
/* end subroutine (checkbase) */


