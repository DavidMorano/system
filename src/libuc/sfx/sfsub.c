/* sfsub SUPPORT */
/* lang=C20 */

/* match a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfsub

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not in the buffer specified by the first two
	arguments.  This subroutine either returns (-1) or it returns
	the length of the found substring.

	Synopsis:
	int sfsub(cchar *sbuf,int slen,cchar *s2,cchar **rpp) noex

	Arguments:
	sbuf	string to be examined
	slen	length of string to be examined
	s2	null terminated substring to search for
	rpp	result pointer of beginning of found string

	Returns:
	>=0	length of found substring
	<0	the substring was not found in the main string buffer

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<strnxcmp.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */


/* exported subroutines */

int sfsub(cchar *sp,int sl,cchar *s2,cchar **rpp) noex {
	int		s2len = 0 ;
	int		f = false ;
	if (sp && s2) {
	    int		i = 0 ;
	    f = true ;
	    s2len = strlen(s2) ;
	    if (s2len > 0) {
		f = false ;
	        if (sl < 0) sl = strlen(sp) ;
	        if (s2len <= sl) {
	            for (i = 0 ; i <= (sl-s2len) ; i += 1) {
		        f = ((s2len == 0) || (sp[i] == s2[0])) ;
	                f = f && (strncmp((sp+i),s2,s2len) == 0) ;
	                if (f) break ;
	            } /* end for */
	        } /* end if (possible) */
	    } /* end if (non-empty) */
	    if (rpp) {
	        *rpp = ((f) ? (sp+i) : nullptr) ;
	    }
	} /* end if (non-null) */
	return (f) ? s2len : -1 ;
}
/* end subroutine (sfsub) */


