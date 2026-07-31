/* sfbasename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the base file name out of a path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfbasename

	Description:
	This routine returns the length of the base name portion
	of the given path string.

	Synopsis:
	int sfbasename(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp	given path string
	sl	length of given path string (can be -1)
	rpp	result pointer of beginning of found string

	Returns:
	>0	length of found string
	==0	not found (no base-name)
	<0	error (fault)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"sfxname.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfbasename(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    while ((sl > 1) && (sp[sl - 1] == '/')) {
	        sl -= 1 ;
	    }
	    {
	        int	si ; /* used-afterwards */
	        for (si = sl ; si > 0 ; si -= 1) {
	            if (sp[si - 1] == '/') break ;
	        } /* end for */
	        if ((sl == 1) && (si == 1) && (sp[0] == '/')) {
	            si -= 1 ;
	        }
	        rp = (sp + si) ;
	        rl = (sl - si) ;
	    } /* end block */
	} /* end if (getlenstr) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end subroutine (sfbasename) */


