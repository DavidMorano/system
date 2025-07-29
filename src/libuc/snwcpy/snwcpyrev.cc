/* snwcpyrev SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy in reverse the characters from a source to a destiation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	snwcpyrev

	Description:
	Copy characters to a destiation string in reverse order
	from a source string.

	Synopsis:
	int snwcpyrev(char *dbuf,int dlen,cchar *sp,int sl) noex

	Arguments:
	dbuf		destination buffer pointer
	dlen		destination buffer length
	sp		soure c-string pointer
	sl		soure c-string length

	Returns:
	>=0		number of characters placed in the destination
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"snwcpyx.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpyrev(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (dbuf && sp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        rs = SR_OVERFLOW ;
	        if (sl < 0) sl = lenstr(sp) ;
	        if ((dlen < 0) || (dlen >= sl)) {
	            int	i ; /* used-afterwards */
	            for (i = 0 ; (i < sl) && sp[i] ; i += 1) {
	                dbuf[i] = sp[sl-i-1] ;
	            } /* end for */
	            dbuf[i] = '\0' ;
		    rs = i ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (snwcpyrev) */


