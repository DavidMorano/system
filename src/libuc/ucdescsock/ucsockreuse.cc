/* ucsockreuse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* reuse an existing (perhaps) socket address */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/socket.h>
#include	<uclibsubs.h>
#include	<localmisc.h>

#include	"ucsock.h"

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

int uc_sockreuse(int fd) noex {
	cint		optlen = szof(int) ;
	cint		sol = SOL_SOCKET ;
	cint		so = SO_REUSEADDR ;
	mut int		one = 1 ; /* <- must be mutable */
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    rs = u_setsockopt(fd,sol,so,&one,optlen) ;
	}
	return rs ;
}
/* end subroutine (uc_sockreuse) */


