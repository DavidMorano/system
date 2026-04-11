/* ucmsgdiscard */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set the message-discard mode on the file descriptor */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/conf.h>
#include	<sys/stat.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_msgdiscard(int fd) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	    rs = u_ioctl(fd,I_SRDOPT,RMSGD) ;
#else
	    rs = SR_NOSYS ;
#endif
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (uc_msgdiscard) */


