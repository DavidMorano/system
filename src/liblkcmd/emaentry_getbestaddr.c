/* emaentry_getbestaddr */

/* get the "best" address out of an EMA-entry address specification */


#define	CF_DEBUGS	0		/* switchable debug print-outs */


/* revision history:

	= 1999-02-01, David A­D­ Morano
        This code was part of another subroutine and I pulled it out to make a
        subroutine that can be used in multiple places.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine extracts the "best" address out of an EMA-entry address
        specification (given in raw string form).


*******************************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>

#include	<usystem.h>
#include	<ema.h>
#include	<localmisc.h>


/* local defines */

#ifndef	EMAENTRY
#define	EMAENTRY	EMA_ENT
#endif


/* external subroutines */


/* local subroutines */


/* exported subroutines */

int emaentry_getbestaddr(EMAENTRY *ep,const char **rpp) noex {
	int	rs = SR_OK ;
	int	cl = 0 ;

	cchar	*cp = NULL ;


	if (ep == NULL) return SR_FAULT ;
	if (rpp == NULL) return SR_FAULT ;

	            if ((ep->rp != NULL) && (ep->rl > 0)) {
	                cp = ep->rp ;
	                cl = ep->rl ;
	            } else if ((ep->ap != NULL) && (ep->al > 0)) {
	                cp = ep->ap ;
	                cl = ep->al ;
	            }

	*rpp = cp ;
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (emaentry_getbestaddr) */



