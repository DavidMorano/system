/* uprogname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uprogname

	Coding note:
	Retrieve the "exec" name of the current process.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |getprogname(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usys.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uprogname.h"


/* local defines */


/* local namespaces */

using libu::sncpy ;			/* subroutine */
using libu::snwcpy ;			/* subroutine */
using libu::sirchr ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_progname(char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    if (rlen > 0) ylikely {
		if (ccharp pn = getprogname()) {
		    if (cint si = sirchr(pn,-1,'.') ; si > 0) {
		        rs = snwcpy(rbuf,rlen,pn,si) ;
		    } else {
		        rs = sncpy(rbuf,rlen,pn) ;
		    } /* end if */
		} else {
		    rbuf[0] = eos ;
		} /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_progname) */


/* local subroutines */


