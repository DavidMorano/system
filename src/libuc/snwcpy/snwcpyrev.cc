/* snwcpyrev SUPPORT */
/* lang=C++20 */

/* copy in reverse the characters from a source to a destiation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Copy characters to a destiation string in reverse order
	from a source string.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ascii.h>

#include	"snwcpyx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* module global variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpyrev(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (dbuf && sp) {
	    rs = SR_OVERFLOW ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((dlen < 0) || (dlen >= sl)) {
	        int	i ; /* used afterwards */
	        for (i = 0 ; (i < sl) && sp[i] ; i += 1) {
	            dbuf[i] = sp[sl-i-1] ;
	        } /* end for */
	        dbuf[i] = '\0' ;
		rs = i ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (snwcpyrev) */


