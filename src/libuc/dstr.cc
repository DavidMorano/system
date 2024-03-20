/* dstr SUPPORT */
/* lang=C++20 */

/* dynamic (allocated) string object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object tries to mimic a dynamic-length string.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"dstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* exported subroutines */

int dstr_start(dstr *sop,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	sop->sbuf = nullptr ;
	sop->slen = 0 ;
	if (sop && sp) {
	    cchar	*rp{} ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = uc_mallocstrw(sp,sl,&rp)) >= 0) {
		sop->slen = rs ;
		sop->sbuf = charp(rp) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (dstr_start) */

int dstr_finish(dstr *sop) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sop) {
	    rs = SR_OK ;
	    if (sop->sbuf) {
	        rs1 = uc_free(sop->sbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        sop->sbuf = nullptr ;
	    }
	    sop->slen = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dstr_finish) */

int dstr_assign(dstr *sop,dstr *sop2) noex {
	int		rs = SR_FAULT ;
	if (sop2) {
	    if ((rs = dstr_finish(sop)) >= 0) {
	        rs = dstr_start(sop,sop2->sbuf,sop2->slen) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dstr_assign) */


