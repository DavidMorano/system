/* mailmsgfrom SUPPORT */
/* lang=C++20 */

/* mail-message-from processing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Process a time value and return whether it is a "hit."

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<strdcpy.h>
#include	<localmisc.h>

#include	"mailmsgfrom.h"


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

int mailmsgfrom_start(mailmsgfrom *op,char *fbuf,int flen) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ; /* dangerous */
	    op->fbuf = fbuf ;
	    op->flen = flen ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgfrom_start) */

int mailmsgfrom_finish(mailmsgfrom *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (mailmsgfrom_finish) */

int mailmsgfrom_test(mailmsgfrom *op,time_t t) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op) {
	    rs = SR_OK ;
	    if (t > op->ti_msg) {
	        f = true ;
	        op->ti_msg = t ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailmsgfrom_test) */

int mailmsgfrom_loadfrom(mailmsgfrom *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		fl = 0 ;
	if (op && sp) {
	    rs = SR_OK ;
	    op->fl = strdcpy1w(op->fbuf,op->flen,sp,sl) - op->fbuf ;
	    fl = op->fl ;
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (mailmsgfrom_loadfrom) */


