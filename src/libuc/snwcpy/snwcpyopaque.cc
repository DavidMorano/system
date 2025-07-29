/* snwcpyopaque SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* special (excellent) string-copy type of subroutine! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpyopaque

	Description:
	This is essentially the same as the |snwcpy(3uc)| subroutine
	except that white-space characters not copied over to the
	result.

	Synopsis:
	int snwcpyopaque(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpycompact(3uc), 
	snwcpyclean(3uc), 
	snwcpyhyphen(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#include	"snwcpyx.h"


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

int snwcpyopaque(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		dl = 0 ; /* return-value */
	if (dbuf && sp) {
	    rs = SR_INVALID ;
	    if (dlen >= 0) {
	        rs = SR_OK ;
	        while (dlen && sl && *sp) {
	            if (cint ch = mkchar(*sp) ; (! CHAR_ISWHITE(ch))) {
		        if (dlen-- == 0) break ;
		        dbuf[dl++] = char(ch) ;
	            } /* end if */
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        if (sl && *sp) rs = SR_OVERFLOW ;
	        dbuf[dl] = '\0' ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpyopaque) */


