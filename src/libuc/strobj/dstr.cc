/* dstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dynamic (allocated) string object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dstr

	Description:
	This little object tries to mimic a dynamic-length string.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"dstr.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dstr_start(dstr *sop,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sop && sp) {
	    sop->sbuf = nullptr ;
	    sop->slen = 0 ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *rp ; (rs = uc_mallocstrw(sp,sl,&rp)) >= 0) ylikely {
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
	if (sop) ylikely {
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
	if (sop && sop2) ylikely {
	    if ((rs = dstr_finish(sop)) >= 0) {
	        rs = dstr_start(sop,sop2->sbuf,sop2->slen) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dstr_assign) */


