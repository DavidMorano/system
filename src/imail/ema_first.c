/* ema_first SUPPORT */
/* lang=C++20 */

/* get first non-empty E-Mail Address */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We get the first non-empty EMA address.


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ema.h"


/* local defines */


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
#endif


/* external variables */


/* local structures */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int ema_first(EMA *op,cchar **rpp) noex {
	EMA_ENT		*ep ;
	int		rs = SR_OK ;
	int		i ;
	int		rl = 0 ;
	cchar		*rp = nullptr ;
	for (i = 0 ; (rs = ema_get(op,i,&ep)) >= 0 ; i += 1) {
	    if (ep != nullptr) {
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
	    if (rpp != nullptr) {
		*rpp = (rl > 0) ? rp : nullptr ;
	    }
	} else if (rs == SR_NOTFOUND) {
	    if (rpp != nullptr) *rpp = nullptr ;
	    rs = SR_OK ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (ema_first) */


