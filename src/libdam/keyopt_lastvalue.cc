/* keyopt_lastvalue SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the last value (among multiple values) from the key-options */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	keyopt_lastvalue

	Description:
	We get the last value (among multiple possible values) from
	the key-options.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<keyopt.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int keyopt_lastvalue(keyopt *op,cchar *kp,cchar **rpp) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ; /*return-value */
	if ((rs = keyopt_magic(op,kp)) >= 0) {
	    cchar	*vp ;
	    vp = nullptr ;
	    vl = 0 ;
	    if (keyopt_cur cur ; (rs = keyopt_curbegin(op,&cur)) >= 0) {
		cchar	*cp ;
	        for (int cl ; (cl = keyopt_fetch(op,kp,&cur,&cp)) >= 0 ; ) {
		    vp = cp ;
		    vl = cl ;
	        } /* end for */
	        rs1 = keyopt_curend(op,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if ((rs >= 0) && (vp == nullptr)) {
	        rs = SR_NOTFOUND ;
	    }
	    if (rpp) {
	        *rpp = (rs >= 0) ? vp : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (keyopt_lastvalue) */


