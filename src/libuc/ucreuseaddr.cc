/* ucreuseaddr */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* reuse an existing (perhaps) socket address */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* exported variables */


/* exported subroutines */

int uc_reuseaddr(int s) noex {
	cint		optlen = sizeof(int) ;
	int		rs = SR_BADFD ;
	mut int		one = 1 ; /* <- must be mutable */
	if (s >= 0) {
	    rs = u_setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&one,optlen) ;
	}
	return (rs >= 0) ? one : rs ;
}
/* end subroutine (uc_reuseaddr) */


