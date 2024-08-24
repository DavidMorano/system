/* ucgetnisdomain SUPPORT */
/* lang=C++20 */

/* get the NIS domain name for the current host */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getnisdomain

	Description:
	Get the NIS domain name for the local host.

	Synopsis:
	int uc_getnisdomain(char *dbuf,int dlen) noex

	Arguments:
	dbuf		buffer to place name in
	dlen		length of user-supplied buffer

	Returns:
	>=0		length of NIS domain name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<usystem.h>


/* local defines */


/* imported namespaces */

using namespace	libu ;			/* |ugetnisdom(3u)| */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getnisdomain(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = ugetnisdom(rbuf,rlen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getnisdomain) */


