/* checkbase SUPPORT */
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
	<0		bad (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<uvariables.hh>
#include	<hasx.h>
#include	<localmisc.h>

#include	"checkbase.h"


/* local defines */


/* imported namespaces */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cint	maxbase = strlen(sysword.w_digtab) ;


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


