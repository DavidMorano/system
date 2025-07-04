/* ecmsg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* email-cover subchannel message */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-13, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ecmsg

	Description:
	This little dittie provides a little way to help manage the
	subchannel message for the email-cover algorithm.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ecmsg.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ecmsg_start(ecmsg *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ecmsg_start) */

int ecmsg_finish(ecmsg *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->ebuf) {
	        rs1 = uc_free(op->ebuf) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    memclear(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ecmsg_finish) */

int ecmsg_loadbuf(ecmsg *op,cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	if (op && mbuf) {
	    if (mlen < 0) mlen = lenstr(mbuf) ;
	    if (mlen > ECMSG_MAXBUFLEN) mlen = ECMSG_MAXBUFLEN ;
	    if (op->ebuf) {
	        uc_free(op->ebuf) ;
	        op->ebuf = nullptr ;
	        op->elen = 0 ;
	    }
	    if (mlen >= 0) {
	        char	*bp ;
	        if ((rs = uc_malloc((mlen+1),&bp)) >= 0) {
		    op->ebuf = bp ;
	            memcpy(bp,mbuf,mlen) ;
	            op->ebuf[mlen] = '\0' ;
	            op->elen = mlen ;
	        }
	    } /* end if (size) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ecmsg_loadbuf) */

int ecmsg_already(ecmsg *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (op->ebuf != nullptr) ;
	}
	return rs ;
}
/* end subroutine (ecmsg_already) */


