/* strnstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a substring within a larger string (case insensitive) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnstr

	Description:
	This subroutine determines if the parameter string (argument
	'ss') is or is not a substring specified by the first
	argument.  This subroutine either returns a pointer to the
	the begining of the found substring or nullptr if not found.
	The string comparisons are case insensitive.

	Synopsis:
	char *strnstr(cchar *sp,cchar *ss,size_t n) noex

	Arguments:
	sp		test c-string pointer
	ss		null terminated substring to search for
	n		maximum number of characters to conider

	Returns:
	-		pointer to found substring
	nullptr		substring was not found
	sp		pointer to 's'1 if 'ss' is zero length

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strnstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern size_t strlen(ccharp) noex ;
} /* end extern (C) */

extern "C" {
    extern int	strncmp(ccharp,ccharp,size_t) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strnstr(cchar *sp,cchar *ss,size_t n) {
    	char		*rp = nullptr ;
	if (sp && ss) ylikely {
	    rp = cast_const<charp>(sp) ;
	    if (csize s2len = strlen(ss) ; s2len > 0) {
	        if (csize sl = strlen(sp) ; sl >= s2len) {
		    char *ep = rp + (sl - s2len) ;
		    bool f = false ;
		    for ( ; rp < ep ; rp += 1) {
			f = (strncmp(rp,ss,n) == 0) ;
			if (f) break ;
		    } /* end for */
		    if (! f) rp = nullptr ;
	        } else {
	            rp = nullptr ;
	        } /* end */
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (strnstr) */


