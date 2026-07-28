/* msghdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* messsage-header methods */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	name:
	msghdr

	Description:
	Message-Header support.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/uio.h>		/* POSIX® |IOVEC| */
#include	<sys/socket.h>		/* POSIX® |MSHGDR| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"msghdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int msghdr_sz(MSGHDR *mhp) noex {
    	int		rs = SR_FAULT ;
	int		sz = 0 ;
	if (mhp) ylikely {
	    IOVEC	*vlp = mhp->msg_iov ;
	    cint	vll = mhp->msg_iovlen ;
	    for (int i = 0 ; i < vll ; i += 1) {
	       sz += intconv(vlp->iov_len) ;
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
} /* end subroutine (msghdr_sz) */


