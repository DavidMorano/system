/* ema_first SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get first non-empty E-Mail Address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	ema_first

	Description:
	We get the first non-empty EMA address.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ema.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ema_first(ema *op,cchar **rpp) noex {
	int		rs ;
	int		rl = 0 ;
	cchar		*rp = nullptr ;
	ema_ent		*ep{} ;
	for (int i = 0 ; (rs = ema_get(op,i,&ep)) >= 0 ; i += 1) {
	    if (ep) {
		if ((ep->rp != nullptr) || (ep->ap != nullptr)) {
		    if (rl == 0) {
			rl = ep->rl ;
			rp = ep->rp ;
		    }
		    if (rl == 0) {
			rl = ep->al ;
			rp = ep->ap ;
		    }
		}
	    }
	    if (rl > 0) break ;
	} /* end for */
	if (rs >= 0) {
	    if (rpp) {
		*rpp = (rl > 0) ? rp : nullptr ;
	    }
	} else if (rs == SR_NOTFOUND) {
	    if (rpp) *rpp = nullptr ;
	    rs = SR_OK ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (ema_first) */


