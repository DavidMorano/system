/* vecobj_recip */
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

	This set of subroutines extends the VECOBJ object (yes we
	are using that to hold "recipients") for some "recipient"
	handling.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<vecobj.h>
#include	<localmisc.h>

#include	"recip.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* exported subroutines */

int vecobj_recipadd(vecobj *op,cchar *np,int nl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (nl < 0) nl = strlen(np) ;
	if (nl > 0) {
	    bool	f ;
	    void	*vp{} ;
	    for (int i = 0 ; (rs = vecobj_get(op,i,&vp)) >= 0 ; i += 1) {
	        if (vp) {
	            RECIP	*rp = (RECIP *) vp ;
	            f = recip_match(rp,np,nl) ;
	            if (f) break ;
	        }
	    } /* end for */
	    if (rs == SR_NOTFOUND) {
	        RECIP	re ;
	        if ((rs = recip_start(&re,np,nl)) >= 0) {
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
		RECIP	*rp = (RECIP *) vp ;
	        rs1 = recip_finish(rp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (vecobj_recipfins) */


