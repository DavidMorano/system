/* suupport_strnset SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* set a string to a number of characters (exactly - not-NUL-terminated) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnset

	Description:
	Copy a specified character to a destination for the number
	of characters specified.  The resulting string is *not*
	NUL-terminated.

	Synopsis:
	char *strnset(cchar *dp,int ch,int n) noex

	Arguments:
	dp	destination string buffer (pointer)
	ch	character to set in the destination string
	n	number of characters

	Returns:
	-	the character pointer to the end of the destination

	Note: 
	This subroutine is similar to:
		void *memset(s,ch,n) ;
	except that a pointer to the end of the string is returned
	instead of a pointer to the beginning of the string!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usupport_strnset.hh"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    char *strnset(char *bp,int ch,int n) noex {
    	if (bp) ylikely {
	    while (n-- > 0) {
	        *bp++ = char(ch) ;
	    }
	} /* end if (non-null) */
	return bp ;
    } /* end subroutine (strnset) */
} /* end namespace (libu) */


