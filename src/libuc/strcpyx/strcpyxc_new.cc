/* strcpyxc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a c-string to destination buffer w/ case conversions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	strcpybc
	strcpylc
	strcpyuc
	strcpyfc

	Description:
	These subroutines copy a source c-string to a (uncounted)
	destination while converting the case of the characters
	(according to which subroutine is called).

	Synopsis:
	char *strcpy{x}c(char *dst,cchar *src) noex

	Arguments:
	dst	destination result buffer (pointer)
	src	source c-string pointer

	Returns:
	-	a pointer to the NUL character in the destination buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<toxc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"strcpyxc.h"


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


