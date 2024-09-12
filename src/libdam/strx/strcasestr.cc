/* strcasestr SUPPORT */
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
	strcasestr

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not a substring specified by the first
	argument.  This subroutine either returns a pointer to the
	the begining of the found substring or nullptr if not found.
	The string comparisons are case insensitive.

	Synopsis:
	char *strcasestr(cchar *s,cchar *s2) noex

	Arguments:
	s		(s1) string to be examined
	s2		null terminated substring to search for

	Returns:
	-		pointer to found substring
	nullptr		substring was not found
	sp		pointer to 's1' if 'ss' is zero length

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<nleadstr.h>
#include	<char.h>
#include	<localmisc.h>

#include	"strx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strcasestr(cchar *sp,cchar *s2) noex {
	cint		s2len = strlen(s2) ;
	char		*rp = (char *) sp ;
	if (s2len > 0) {
	    cint	sl = strlen(sp) ;
	    if (s2len <= sl) {
	        cint	s2lead = CHAR_TOLC(s2[0]) ;
		int	m ;
		int	i ; /* used-afterwards */
		bool	f = false ;
	        for (i = 0 ; i <= (sl-s2len) ; i += 1) {
		    int	ch = CHAR_TOLC(sp[i]) ;
	            if (ch == s2lead) {
	                m = nleadcasestr((sp+i),s2,s2len) ;
	                f = (m == s2len) ;
			if (f) break ;
	            } /* end if */
	        } /* end for */
	        rp = (char *) ((f) ? (sp+i) : nullptr) ;
	    } else {
	        rp = nullptr ;
	    }
	} /* end if (positive) */
	return rp ;
}
/* end subroutine (strcasestr) */


