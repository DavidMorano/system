/* ucfiledesc SUPPORT */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
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
	int		rs ;
	va_list		ap ;
	va_begin(ap,cmd) ;
	{
	    caddr_t	any = va_arg(ap,caddr_t) ;
	    rs = u_fcntl(fd,cmd,any) ;
	}
	va_end(ap) ;
	return rs ;
}
/* end subroutine (uc_fcntl) */

int uc_ioctl(int fd,int cmd,...) noex {
	int		rs ;
	va_list		ap ;
	va_begin(ap,cmd) ;
	{
	    caddr_t	any = va_arg(ap,caddr_t) ;
	    rs = u_ioctl(fd,cmd,any) ;
	}
	va_end(ap) ;
	return rs ;
}
/* end subroutine (uc_ioctl) */

int uc_tell(int fd,off_t *offp) noex {
	return u_tell(fd,offp) ;
}
/* end subroutine (uc_tell) */


