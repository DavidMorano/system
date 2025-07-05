/* vecobj_recip SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extend the VECOBJ object w/ some recipient handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was completely rewritten to replace the
	previous mail-delivery program for PCS, written around 1990
	or so and which also served for a time to filter environment
	for the SENDMAIL daemon.

	= 2004-02-17, David A­D­ Morano
	This was modified to add the MSGID object.  That is a
	database that stores message IDs.  We used it to eliminate
	duplicate mail deliveries which as of late are coming from
	several popular sources!

*/

/* Copyright © 1998,2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecobj_recip

	Description:
	This set of subroutines extends the VECOBJ object (yes we
	are using that to hold "recipients") for some "recipient"
	handling.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<estrings.h>
#include	<vecobj.h>
#include	<localmisc.h>

#include	"recip.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	vecobj_reciphave(vecobj *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int vecobj_recipadd(vecobj *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (sl < 0) sl = lenstr(sp) ;
	if (sl > 0) {
	    if ((rs = vecobj_reciphave(op,sp,sl)) == 0) {
	        if (RECIP re ; (rs = recip_start(&re,sp,sl)) >= 0) {
		    c = 1 ;
	            rs = vecobj_add(op,&re) ;
	            if (rs < 0) {
	                recip_finish(&re) ;
		    }
	        } /* end if (new recip) */
	    } /* end if (new recipient) */
	} /* end if (positive) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecobj_recipadd) */

int vecobj_recipfins(vecobj *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		recip	*rp = (recip *) vp ;
	        rs1 = recip_finish(rp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (vecobj_recipfins) */


/* local subroutines */

static int vecobj_reciphave(vecobj *op,cchar *sp,int sl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	void		*vp{} ;
	if (sl < 0) sl = lenstr(sp) ;
	for (int i = 0 ; (rs1 = vecobj_get(op,i,&vp)) >= 0 ; i += 1) {
	    recip	*rp = (recip *) vp ;
	    if (vp) {
	        f = recip_match(rp,sp,sl) ;
	        if (f) break ;
	    }
	} /* end for */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (vecobj_reciphave) */



