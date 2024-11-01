/* ema_haveaddr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* EMA have a given address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ema_haveaddr

	Description:
	This subroutine determines if a given address is inside an
	EMA.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ema.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ema_haveaddr(EMA *emap,cchar *ap,int al) noex {
	EMA_ENT		*ep ;
	int		rs = SR_OK ;
	int		f = false ;
	if (al < 0) al = strlen(ap) ;
	for (int i = 0 ; (rs = ema_get(emap,i,&ep)) >= 0 ; i += 1) {
	    int		cl = 0 ;
	    cchar	*cp = nullptr ;
	    if ((ep->rp != nullptr) && (ep->rl > 0)) {
		cp = ep->rp ;
		cl = ep->rl ;
	    } else if ((ep->ap != nullptr) && (ep->al > 0)) {
		cp = ep->ap ;
		cl = ep->al ;
	    }
	    if (cp != nullptr) {
		f = (cl == al) ;
		f = f && (strncmp(cp,ap,al) == 0) ;
		if (f) break ;
	    }
	} /* end for */
	if (rs == SR_NOTFOUND) rs = SR_OK ;
	return (rs >= 0) ? f : rs ;
} 
/* end subroutine (ema_haveaddr) */


