/* sfprogname SUPPORT */
/* lang=C++20 */

/* try to find a "program" name out of a given c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfprogname

	Description:
	This routine returns the length of what it thinks it the
	"program-name" extracted from the given c-string.

	Synopsis:
	int sfprogname(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp	given path c-string pointer
	sl	given path c-string length (can be -1)
	rpp	result pointer of beginning of found string

	Returns:
	>0	length of found string
	==0	not found (no base-name)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>		/* <- not-needed */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<six.h>

#include	"sfx.h"


/* exported subroutines */

int sfprogname(cchar *sp,int sl,cchar **rpp) noex {
	int		cl = 0 ;
	cchar		*cp{} ;
	if ((cl = sfbasename(sp,sl,&cp)) > 0) {
	    int		si ;
	    if ((si = sirchr(cp,cl,'.')) >= 0) {
		cl -= si ;
	    }
	    if ((cl > 0) && (cp[0] == '=')) {
		cp += 1 ;
		cl -= 1 ;
	    }
	} /* end if (sfbasename) */
	if (rpp) *rpp = cp ;
	return cl ;
}
/* end subroutine (sfprogname) */


