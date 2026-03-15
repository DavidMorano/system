/* haseol SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* does the c-string contain a leading given character (after skipping WS) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

	= 2017-03-21, David A-D- Morano
	I made some sort of update.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	haslead

	Description:
	Determine if the given c-string contains a leading character
	(caller supplied) after skipping all leading white-space.

	Synopsis:
	bool haslead(cchar *sp,int µsl,int chx) noex

	Arguments:
	sp		c-string pointer
	sl		c-string length
	chx		character to match to

	Returns:
	true		the given c-string has a leading given character
	false		the given c-string did not match

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"haseol.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

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

bool haseol(cchar *sp,int µsl) noex {
	bool		f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    cint ch = sp[sl - 1] ;
	    f = (ch == CH_NL) || (ch == CH_CR) ;
	} /* end if (getlenstr) */
	return f ;
} /* end subroutine (haslead) */


