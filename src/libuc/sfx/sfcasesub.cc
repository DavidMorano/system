/* sfcasesub SUPPORT */
/* lang=C++20 */

/* match a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfcasesub

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not in the buffer specified by the first two
	arguments. This subroutine either returns (-1) or it returns
	the length of the found substring.

	Synopsis:
	int sfcasesub(cchar *s1,int len,cchar *s2,cchar **rpp) noex

	Arguments:
	s1	string to be examined
	len	length of string to be examined
	s2	null terminated substring to search for
	rpp	result pointer of beginning of found string

	Returns:
	>=0	length of found substring
	<0	the substring was not found in the main string buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<nleadstr.h>		/* |nleadcasestr(3uc)| */
#include	<char.h>
#include	<toxc.h>

#include	"sfx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfcasesub(cchar *sp,int sl,cchar *s2,cchar **rpp) noex {
	int		s2len = 0 ;
	bool		f = false ;
	if (sp && s2) {
	    int		i = 0 ;
	    f = true ;
	    s2len = strlen(s2) ;
	    if (s2len > 0) {
	        f = false ;
	        if (sl < 0) sl = strlen(sp) ;
	        if (s2len <= sl) {
	            cint	s2lead = CHAR_TOLC(s2[0]) ;
	            for (i = 0 ; i <= (sl-s2len) ; i += 1) {
	                if (CHAR_TOLC(sp[i]) == s2lead) {
		            cint	m = nleadcasestr((sp+i),s2,s2len) ;
		            f = (m == s2len) ;
			    if (f) break ;
	                }
	            } /* end for */
	        } /* end if (possible) */
	    } /* end if (non-zero string) */
	    if (rpp) {
	        *rpp = (f) ? (sp + i) : nullptr ;
	    }
	} /* end if (non-null) */
	return (f) ? s2len : -1 ;
}
/* end subroutine (sfcasesub) */


