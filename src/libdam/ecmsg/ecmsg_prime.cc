/* ecmsg_prime SUPPORT */
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ecmsg.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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
	if (op) ylikely {
	    rs = memclear(op) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ecmsg_start) */

int ecmsg_finish(ecmsg *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->ebuf) {
	        rs1 = lm_free(op->ebuf) ;
	        if (rs >= 0) rs = rs1 ;
		op->ebuf = nullptr ;
	    } /* end if (memory-release) */
	    memclear(op) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ecmsg_finish) */

int ecmsg_loadbuf(ecmsg *op,cchar *mbuf,int mlen) noex {
	int		rs = SR_FAULT ;
	if (op && mbuf) ylikely {
	    rs = SR_OK ;
	    if (mlen < 0) mlen = lenstr(mbuf) ;
	    if (mlen > ECMSG_MAXBUFLEN) {
		mlen = ECMSG_MAXBUFLEN ;
	    }
	    if (op->ebuf) {
	        lm_free(op->ebuf) ;
	        op->ebuf = nullptr ;
	        op->elen = 0 ;
	    } /* end if (memory-release) */
	    if (mlen >= 0) {
	        if (char *bp ; (rs = lm_mall((mlen + 1),&bp)) >= 0) {
		    op->ebuf = bp ;
	            strwcpy(bp,mbuf,mlen) ;
	            op->elen = mlen ;
	        } /* end if (memory-allocation) */
	    } /* end if (size) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ecmsg_loadbuf) */

int ecmsg_already(ecmsg *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = (op->ebuf != nullptr) ;
	}
	return rs ;
} /* end subroutine (ecmsg_already) */


