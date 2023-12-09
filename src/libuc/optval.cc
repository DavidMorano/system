/* optval */
/* lang=C++20 */

/* Option-Value (including boolean) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<matostr.h>
#include	<mkchar.h>
#include	<cfnum.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"optval.h"


/* local defines */


/* external subroutines */


/* local variables */

static constexpr cchar	*hits[] = {
	"0",
	"1",
	"no",
	"yes",
	"false",
	"true",
	"-",
	"+",
	nullptr
} ;


/* exported subroutines */

int optval(cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		v = 0 ;
	if (sp) {
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (sl > 0) {
		int	hi ;
	        if ((hi = matocasestr(hits,1,sp,sl)) >= 0) {
	            v = (hi & 1) ;
	        } else {
	            int		ch = mkchar(sp[0]) ;
		    rs = SR_INVALID ;
	            if (isdigitlatin(ch)) {
	                rs = cfnumi(sp,sl,&v) ;
		    }
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (optval) */


