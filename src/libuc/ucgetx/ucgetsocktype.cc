/* ucgetsocktype SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* set the NONBLOCK file descriptor open-flag */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_getsocktype

	Description:
	We retrieve a socket type (second argument to |socket(3xnet)|)
	given a socket by way of a file-descriptor.

	Synopsis:
	int uc_getsocktype(int fd) noex

	Arguments:
	fd		file-descriptor

	Returns:
	>=0		socket type number
	<0		error (like "not-a-socket")

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


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

int uc_getsocktype(int fd) noex {
	cint		slev = SOL_SOCKET ;
	cint		scmd = SO_TYPE ;
	int		rs ;
	int		tval = 0 ;
	int		tlen = szof(int) ;
	if ((rs = u_getsockopt(fd,slev,scmd,&tval,&tlen)) >= 0) {
	    if (tlen != szof(int)) rs = SR_NOMSG ;
	}
	return (rs >= 0) ? tval : rs ;
}
/* end subroutine (uc_getsocktype) */


