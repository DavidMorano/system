/* ourmsginfo SUPPORT (Our_Message-Information) */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load and store certain mail-message information from the headers */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Object:
	ourmsginfo

	Description:
	This subroutine will load and store certain mail-message
	information as gleaned from the various mail-message headers.

	Synopsis:
	int ourmsginfo_start(ourmsginfo *op,dater *dp) noex

	Arguments:
	op		pointer to object
	dp		pointer to DATER object

	Returns:
	>=0		length of found filepath
	<0		error (system-return)

*****************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<vecstr.h>
#include	<dater.h>
#include	<localmisc.h>

#include	"ourmsginfo.h"


/* local defines */

#define	OURMSGINFO_N	10


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ourmsginfo_start(ourmsginfo *op,dater *dp) noex {
    	OURMSGINFO	*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(hop) ;
	    rs = dater_startcopy(&op->edate,dp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ourmsginfo_start) */

int ourmsginfo_finish(ourmsginfo *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < ourmsginfohead_overlast ; i += 1) {
	        if ((op->hif >> i) & 1) {
	            rs1 = vecstr_finish((op->head + i)) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = dater_finish(&op->edate) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ourmsginfo_finish) */

int ourmsginfo_addhead(ourmsginfo *op,int w,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    rs = SR_INVALID ;
	    if ((w >= 0) && (w < ourmsginfohead_overlast)) {
		if (sl < 0) sl = cstrlen(sp) ;
		if (sl > 0) {
		    vecstr	*vsp = (op->head + w) ;
		    /* is the vector for this header initialized? */
	            if (! ((op->hif >> w) & 1)) {
	                cint	vo = 0 ;
	                rs = vsp->start(OURMSGINFO_N,vo) ;
	                op->hif |= (1 << w) ;
	            } /* end if (needed initialization) */
	            if ((rs >= 0) && (sl > 0)) {
	                rs = vsp->add(sp,sl) ;
	            }
		} /* end if (non-zero positive) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ourmsginfo_addhead) */

int ourmsginfo_gethead(ourmsginfo *op,int w,int i,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if ((w >= 0) && (w < ourmsginfohead_overlast)) {
		vecstr	*vsp = (op->head + w) ;
		rs = vsp->get(i,rpp) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ourmsginfo_gethead) */


