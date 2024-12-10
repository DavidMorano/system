/* sfprogname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* try to find a "program" name out of a given c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-17, David A­D­ Morano
	This code was originally written.

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- not-needed */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<six.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */


/* exported variables */


/* exported subroutines */

int sfprogname(cchar *sp,int sl,cchar **rpp) noex {
	int		cl = 0 ;
	cchar		*cp{} ;
	if ((cl = sfbasename(sp,sl,&cp)) > 0) {
	    if (int si ; (si = sirchr(cp,cl,'.')) >= 0) {
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


