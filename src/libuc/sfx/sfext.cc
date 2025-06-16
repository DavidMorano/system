/* sfext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-find a file-name extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfext

	Description:
	This subroutine will look for and return the extension
	of a given file-name (given as a counted c-string), if
	one exits.

	Synopsis:
	int sfext(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		base source c-string pointer
	sl		base source c-string length
	rpp		pointer to result pointer to store found value

	Returns:
	>=0		length of c-string after any found
	==0		a trailing dot character was found (only)
	<0		error (SR_NOTFOUND)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<six.h>			/* |sirchr(3uc)| */
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"sfx.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfext(cchar *sp,int sl,cchar **rpp) noex {
    	int		rs = SR_FAULT ;
	int		el = 0 ; /* return-value */
	if (sp && rpp) {
	    cchar	*bp ;
	    cchar	*ep = nullptr ;
	    bool	f = false ;
	    rs = SR_OK ;
	    if (int bl ; (bl = sfbasename(sp,sl,&bp)) > 0) {
		if (int si ; (si = sirchr(bp,bl,'.')) > 0) {
		    f = true ;
		    el = (bl - (si + 1)) ;
		    ep = (bp + (si + 1)) ;
		}
	    }
	    *rpp = (f) ? ep : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (sfext) */


