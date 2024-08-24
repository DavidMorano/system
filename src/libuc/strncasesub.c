/* strncasesub */
/* lang=C20 */

/* find a substring within a larger string (case insensitive) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strncasesub

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not a substring specified by the first two
	arguments. This subroutine either returns a pointer to the
	the begining of the found substring or NULL if not found.
	The string comparisons are case insensitive.

	Synopsis:
	char *strncasesub(cchar *sp,int sl,cchar *s2) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined
	s2	NUL-terminated substring to search for

	Returns:
	-	pointer to found substring
	NULL	substring was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<localmisc.h>

#include	"strn.h"


/* local defines */


/* external subroutines */

extern int	nleadcasestr(cchar *,cchar *,int) noex ;


/* exported subroutines */

char *strncasesub(cchar *sp,int sl,cchar *s2) noex {
	char		*rp = nullptr ;
	if (sp && s2) {
	    cint	s2len = strlen(s2) ;
	    rp = (char *) sp ;
	    if (s2len > 0) {
	        if (sl < 0) sl = strlen(sp) ;
	        if (s2len <= sl) {
		    cint	s2lead = CHAR_TOLC(s2[0]) ;
	            int		i ;
		    int		f = false ;
	            for (i = 0 ; i <= (sl-s2len) ; i += 1) {
	                if (CHAR_TOLC(sp[i]) == s2lead) {
	                    cint	m = nleadcasestr((sp+i),s2,s2len) ;
	                    f = (m == s2len) ;
			    if (f) break ;
	                } /* end if */
	            } /* end for */
	            rp = (char *) ((f) ? (sp+i) : nullptr) ;
	        } else {
	            rp = nullptr ;
	        }
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strncasesub) */


