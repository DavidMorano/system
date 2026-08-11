/* cmdutils1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Files-Utilities (for FILES program) */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-03-21, David A-D- Morano
	This subroutine was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	optval

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |cfdec(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

module cmdutils ;

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* is{x}(2u)| */

/* local defines */


/* imported namespaces */

using libu::cfdec ;			/* |cfdec(3u)| */
using libu::matocasestr ;		/* |matocasestr(3u)| */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cpcchar	hits[] = {
	"0",
	"1",
	"no",
	"yes",
	"false",
	"true",
	"-",
	"+",
	nullptr
} ; /* end array (hits) */


/* exported variables */


/* exported subroutines */

int optval(cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) ylikely {
	    rs = SR_OK ;
	    if (sl > 0) ylikely {
		if (int hi ; (hi = matocasestr(hits,1,sp,sl)) >= 0) {
	            v = (hi & 1) ;
	        } else {
	            cint	ch = mkchar(sp[0]) ;
		    rs = SR_INVALID ;
	            if (isnumlatin(ch)) {
	                rs = cfdec(sp,sl,&v) ; /* <- swapped out */
		    }
	        } /* end if */
	    } /* end if (non-zero positive) */
	} /* end if (getlenstr) */
	return (rs >= 0) ? v : rs ;
} /* end subroutine (optval) */


