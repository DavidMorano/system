/* ucdescmisc SUPPORT */
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
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucdescbase.hh"
#include	"ucdescmisc.h"


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
} /* end subroutine (uc_fcntl) */

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
} /* end subroutine (uc_ioctl) */

int uc_seeko(int fd,off_t fo,int cmd,off_t *resp) noex {
    	return u_seeko(fd,fo,cmd,resp) ;
} /* end subroutine */

int uc_tell(int fd,off_t *offp) noex {
	return u_tell(fd,offp) ;
} /* end subroutine (uc_tell) */

int uc_fchown(int fd,uid_t uid,gid_t gid) noex {
	return u_fchown(fd,uid,gid) ;
} /* end subroutine (uc_fchown) */

int uc_fchmod(int fd,mode_t m) noex {
	return u_fchmod(fd,m) ;
} /* end subroutine (uc_fchown) */

int uc_fpathconf(int fd,int cmd,long *rp) noex {
	return u_fpathconf(fd,cmd,rp) ;
} /* end subroutine (uc_fpathconf) */


