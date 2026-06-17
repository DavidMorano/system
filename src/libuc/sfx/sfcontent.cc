/* sfcontent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find any sub-string that contains something (ignoring comments) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfcontent

	Description:
	This subroutine will search for some non-whitespace
	characters before any possible comment characters.

	Synopsis:
	int sfcontent	(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp		test string pointer
	sl		test string length
	rpp		pointer to result pointer to store found value

	Returns:
	>=0		length of found content c-string
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<six.h>			/* LIBUC |sichr(3uc)| */
#include	<strwcmp.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC |iseol(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sfx.h"			/* |sfshrink(3uc)| + |sfnext(3uc)| */
#include	"sfcontent.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* lenstr(3u) */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfcontent(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (int sl = getlenstr(sp,µsl) ; sl >= 0) ylikely {
	    rl = 0 ;
	    if (sl > 0) ylikely {
	        if (int si = sichr(sp,sl,'#') ; si >= 0) {
		    sl = si ;
	        } else {
		    while (sl && iseol(sp[sl - 1])) {
			sl -= 1 ;
		    } /* end while */
	        } /* end if (comment or EOL) */
		if (sl) {
		    rl = sfshrink(sp,sl,&rp) ;
		} else {
		    rp = sp ;
		}
	    } /* end if (non-zero positive) */
	} /* end if (getlenstr) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end subroutine (sfcontent) */


