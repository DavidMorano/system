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

int ema_haveaddr(ema *op,cchar *ap,int al) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (op && ap) {
	    ema_ent	*ep{} ;
	    if (al < 0) al = cstrlen(ap) ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs1 = ema_get(op,i,&ep)) >= 0 ; i += 1) {
	        int	cl = 0 ;
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
	    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	} /* end if (null-pointer) */
	return (rs >= 0) ? f : rs ;
} 
/* end subroutine (ema_haveaddr) */


