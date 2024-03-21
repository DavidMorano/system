/* mailmsgfrom SUPPORT */
/* lang=C++20 */

/* mail-message-from processing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgfrom_start(MAILMSGFROM *op,char *fbuf,int flen) noex {
	if (op == NULL) return SR_FAULT ;
	memclear(op) ; /* dangerous */
	op->fbuf = fbuf ;
	op->flen = flen ;
	return SR_OK ;
}
/* end subroutine (mailmsgfrom_start) */

int mailmsgfrom_finish(MAILMSGFROM *op) noex {
	if (op == NULL) return SR_FAULT ;
	return SR_OK ;
}
/* end subroutine (mailmsgfrom_finish) */

int mailmsgfrom_test(MAILMSGFROM *op,time_t t) noex {
	int		rs = SR_OK ;
	int		f = FALSE ;
	if (op == NULL) return SR_FAULT ;
	if (t > op->ti_msg) {
	    f = TRUE ;
	    op->ti_msg = t ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailmsgfrom_test) */

int mailmsgfrom_loadfrom(MAILMSGFROM *op,cchar *sp,int sl) noex {
	if (op == NULL) return SR_FAULT ;
	if (sp == NULL) return SR_FAULT ;
	op->fl = strdcpy1w(op->fbuf,op->flen,sp,sl) - op->fbuf ;
	return op->fl ;
}
/* end subroutine (mailmsgfrom_loadfrom) */


