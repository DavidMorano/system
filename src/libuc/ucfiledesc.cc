/* ucfiledesc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uc_seeko
	uc_fcntl

  	Description:
	No, we are not the same as UNIX® all of the time!  We return
	PFNOSUPPORT is a protocol-family (PF) is not supported!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucfiledesc.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_fcntl(int fd,int cmd,...) noex {
	va_list		ap ;
	int		rs = SR_INVALID ;
	if (cmd >= 0) {
	    va_begin(ap,cmd) ;
	    caddr_t	any = va_arg(ap,caddr_t) ;
	    rs = u_fcntl(fd,cmd,any) ;
	    va_end(ap) ;
	} /* end if */
	return rs ;
}
/* end subroutine (uc_fcntl) */

int uc_ioctl(int fd,int cmd,...) noex {
	va_list		ap ;
	int		rs = SR_INVALID ;
	if (cmd >= 0) {
	    va_begin(ap,cmd) ;
	    caddr_t	any = va_arg(ap,caddr_t) ;
	    rs = u_ioctl(fd,cmd,any) ;
	    va_end(ap) ;
	} /* end if */
	return rs ;
}
/* end subroutine (uc_ioctl) */

int uc_tell(int fd,off_t *offp) noex {
	return u_tell(fd,offp) ;
}
/* end subroutine (uc_tell) */


