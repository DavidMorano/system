/* mkchar SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* make a character (8-bit) out of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This was written for Rightcore Network Services (RNS).

*/

/* Copyright (c) 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkchar

	Description:
	We make a character (an 8-bit entity) out of an integer.
	We do this quite simply.

	Synopsis:
	int mkchar(int ch) noex

	Arguments:
	ch		a character

	Returns:
	-		a real character w/o any junk

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* |UCHAR_MAX| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"mkchar.h"


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

#ifndef	MKCHAR_STATICINLINE
inline int mkchar(int ch) noex {
	return (ch & UCHAR_MAX) ;
}
/* end subroutine (mkchar) */
#endif /* MKCHAR_STATICINLINE */


