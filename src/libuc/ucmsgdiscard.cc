/* ucmsgdiscard */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set the message-discard mode on the file descriptor */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/conf.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/uio.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


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
} /* end subroutine (uc_msgdiscard) */


