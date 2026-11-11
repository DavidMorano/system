/* strabbr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-number abbreviations */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strabbr

	Description:

	Synopsis:
	cchar *strabbr(int n) noex

	Arguments:
	n		system-error return number to lookup

	Returns:
	-		character-string representation of system-error return

	Notes:
	I make no attænpt to put any kind of mutex-lock on the
	static buffer.  he current API does not allow for it in any
	case.  No major harm is done if some races occurs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |ctdec(3u)| */
#include	<ucodenames.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |DECBUFLEN| */

#include	"strabbr.h"


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


