/* optval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Option-Value (including boolean) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	optval

	Description:
	Get (divine) an option value or boolean.

	Synopsis:
	int optval(cchar *sp,int sl) noex

	Arguments:
	sp		c-string pointer
	sl		c-string length

	Returns:
	>=0		value
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |lenstr(3c)| */
#include	<usystem.h>
#include	<matostr.h>
#include	<cfnum.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"optval.h"

import libutil ;

/* local defines */


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

int optval(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (sp) {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl > 0) {
		if (int hi ; (hi = matocasestr(hits,1,sp,sl)) >= 0) {
	            v = (hi & 1) ;
	        } else {
	            cint	ch = mkchar(sp[0]) ;
		    rs = SR_INVALID ;
	            if (isnumlatin(ch)) {
	                rs = cfnumi(sp,sl,&v) ;
		    }
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (optval) */


