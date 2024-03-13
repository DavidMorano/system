/* ucopen SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	No, we are not the same as UNIX® all of the time!  We return
	PFNOSUPPORT is a protocol-family (PF) is not supported!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucopen.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported subroutines */

int uc_socket(int pf,int pt,int proto) noex {
	return u_socket(pf,pt,proto) ;
}
/* end subroutine (uc_socket) */


