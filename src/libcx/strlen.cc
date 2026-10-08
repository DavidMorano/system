/* strlen SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate the length of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1982-09-10, David A­D­ Morano
	This subroutine was written because I need this on our own
	embedded (VMS CPU) platform.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	strlen

	Description:
	This subroutine is a knock off of the |strlen(3c)| from the
	regular UNIX® system.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* external subroutines */

size_t strlen(cchar *s) noex {
	size_t		len = 0 ;
	if (s) {
	    while (*s++) len += 1 ;
	} /* end if */
	return len ;
} /* end subroutine (strlen) */


