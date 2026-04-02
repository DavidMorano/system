/* usupport_getsign SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsign

	Description:
	Convert a binary integer to its decimal representation.

	Synopsis:
    	int getsign(cchar *sp,int µsl,bool *fnegp) noex

	Arguments:
	sp	source c-string pointer
	sl	source c-string length
	fnegp	pointer to hold boolean result

	Returns:
	>=0	skip index (number of characters skipped)
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"usupport_getsign.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import ischx ;				/* character classification */

/* local defines */

#define	ischwht(ch)		ischwhite(ch)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int getsign(cchar *sp,int µsl,bool *fnegp) noex {
	int		rs = SR_FAULT ;
	int		skiplen = 0 ; /* return-value (remaining-length) */
        if (int sl ; fnegp && ((sl = getlenstr(sp,µsl)) >= 0)) ylikely {
	    cint slen = sl ; /* starting length */
	    rs = SR_OK ;
            while ((sl > 0) && ischwht(*sp)) {
                sp += 1 ;
                sl -= 1 ;
            }
            if ((sl > 0) && ischpm(*sp)) {
                *fnegp = (*sp == '-') ;
                sp += 1 ;
                sl -= 1 ;
            }
            while ((sl > 0) && ischwht(*sp)) {
                sp += 1 ;
                sl -= 1 ;
            }
            skiplen = (slen - sl) ;
        } /* end if (valid) */
	return (rs >= 0) ? skiplen : rs ;
    } /* end if (getsign) */
} /* end namespace (libu) */


/* local subroutines */


