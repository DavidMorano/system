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
#include	<sys/uio.h>		/* |IOVEC| */
#include	<sys/socket.h>		/* |MSHGDR| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<strwcpy.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int msghdr_size(MSGHDR *mhp) noex {
    	int		rs = SR_FAULT ;
	int		sz = 0 ;
	if (mhp) {
	    IOVEC	*vlp = mhp->msg_iov ;
	    int		vll = mhp->msg_iovlen ;
	    for (int i = 0 ; i < vll ; i += 1) {
	       sz += intconv(vlp->iov_len) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (msghdr_size) */


