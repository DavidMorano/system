/* strnsub */
/* lang=C20 */

/* find a substring within a larger string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnsub

	Description:
	This subroutine determines if the parameter string (argument
	's2') is or is not a substring specified by the first two
	arguments.  This subroutine either returns a pointer to the
	the begining of the found substring or nullptr if not found.

	Synopsis:
	char *strnsub(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined
	ss	null terminated substring to search for

	Returns:
	-	pointer to found substring
	nullptr	substring was not found

*******************************************************************************/

#include	<envstandards.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"strn.h"


/* local defines */


/* forward references */

static char	*strnsub_local(cchar *,int,cchar *) noex ;


/* local variables */


/* exported subroutines */

char *strnsub(cchar *sp,int sl,cchar *ss) noex {
	char		*rp = nullptr ;
	if (sp && ss) {
	    if (sl >= 0) {
	        rp = strnsub_local(sp,sl,ss) ;
	    } else {
	        rp = strstr(sp,ss) ;
	    }
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strnsub) */


/* local subroutines */

char *strnsub_local(cchar *sp,int sl,cchar *s2) noex {
	cint		s2len = strlen(s2) ;
	char		*rp = (char *) sp ;
	if (s2len > 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (s2len <= sl) {
	        int	i ;
		bool	f = false ;
	        for (i = 0 ; i <= (sl-s2len) ; i += 1) {
	            f = ((s2len == 0) || (sp[i] == s2[0])) ;
	            f = f && (strncmp((sp+i),s2,s2len) == 0) ;
		    if (f) break ;
	        } /* end for */
	        rp = (char *) ((f) ? (sp+i) : nullptr) ;
	    } else {
	        rp = nullptr ;
	    }
	} /* end if (positive) */
	return rp ;
}
/* end subroutine (strnsub_local) */


