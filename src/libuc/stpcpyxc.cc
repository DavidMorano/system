/* stpcpyxc SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */

#define	CF_STPCPY	1		/* use |stpcpy(3c)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	stpcpybc
	stpcpylc
	stpcpyuc
	stpcpyfc

	Description:
	These subroutines copy a source c-string to a (uncounted)
	destination while converting the case of the characters
	(according to which subroutine is called).

	Synopsis:
	char *stpcpy{x}c(char *dst,cchar *src) noex

	Arguments:
	dst	destination result buffer (pointer)
	src	source c-string pointer

	Returns:
	-	a pointer to the NUL character in the destination buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"stpcpyxc.h"


/* local defines */

#ifndef	CF_STPCPY
#define	CF_STPCPY	1		/* use |stpcpy(3c)| */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


