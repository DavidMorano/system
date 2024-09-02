/* snclean SUPPORT */
/* lang=C++20 */

/* clean up a c-string of text */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snclean

	Description:
	This subroutine cleans up a counted c-string.

	Synopsis:
	int snclean(char *dbuf,int dlen,cchar *sp,int sl) noex

	Arguments:
	dbuf		result buffer pointer
	dlen		result buffer length
	sp		source c-string pointer
	sl		source c-string length

	Returns:
	>=0		length of result c-string
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"snclean.h"


/* local defines */


/* external subroutines */


/* external variables */


/* external variables */


/* local structures */


/* forward references */

static bool	ischarok(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int snclean(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		dl = 0 ;
	if (dbuf && sp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        rs = SR_OK ;
	        if (sl < 0) sl = INT_MAX ;
	        for (int i = 0 ; (rs >= 0) && (i < sl) && sp[i] ; i += 1) {
	            cint	ch = mkchar(sp[i]) ;
		    if (i < dlen) {
	                if (isprintlatin(ch) || ischarok(ch)) {
		            dbuf[i] = sp[i] ;
		            dl += 1 ;
	                } else {
		            dbuf[i] = '¿' ;
		            dl += 1 ;
	                }
		    } else {
			rs = SR_OVERFLOW ;
		    }
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snclean) */


/* local subroutines */

static bool ischarok(int ch) noex {
	return ((ch == '\t') || (ch == '\n')) ;
}
/* end subroutine (ischarok) */


