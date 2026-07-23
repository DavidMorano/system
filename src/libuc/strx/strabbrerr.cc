/* strabbrerr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return an abbreviation string given a system-error return number */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strabbrerr

	Description:
	We take a system-error return number and we return a
	corresponding abbreviation string.

	Synopsis:
	cchar *strabbrerr(uint n) noex

	Arguments:
	n		system-error return number to lookup

	Returns:
	-		character-string representation of system-error return

	Notes:
	I make no attænpt to put any kind of mutex-lock on the
	static buffer.  he current API does not allow for it in any
	case.  No major harm is done if some races occurs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |ctdec(3u)| */
#include	<ucodenames.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |DECBUFLEN| */

#include	"strabbrerr.h"


/* local defines */


/* local namespaces */

using libu::ctdec ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cint		oklen = DECBUFLEN ;
static char		okbuf[DECBUFLEN+1] ;


/* exported variables */


/* exported subroutines */

cchar *strabbrerr(int rsv) noex {
	cchar		*s = nullptr ;
	if (rsv >= 0) {
	    ctdec(okbuf,oklen,rsv) ;
	    s = okbuf ;
	} else {
	    int rs ;
	    if (cchar *rp ; (rs = ucodename_sr(rsv,&rp)) >= 0) {
		s = rp ;
	    } else {
	        s = "*UNK*" ;
	    }
	} /* end if */
	return s ;
} /* end subroutine (strabbrerr) */


