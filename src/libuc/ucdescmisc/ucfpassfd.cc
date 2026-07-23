/* ucfpassfd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* pass a file-descriptor to a file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_fpassfd

	Description:
	We pass a file-descriptor to another file-descriptor.

	Synopsis:
	int uc_fpassfd(int pfd,int fd) noex

	Arguments:
	pfd		pass-file-descriptor
	fd		file-descriptor to pass

	Returns:
	>=0		OK
	<0		error (shystem-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucfpassfd.h"

/* local defines */


/* external subroutines */


/* external variables */


/* forward reference */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_fpassfd(int fd_pass,int fd) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd_pass,&sb)) >= 0) {
	    if (S_ISCHR(sb.st_mode) || S_ISFIFO(sb.st_mode)) {
		rs = u_ioctl(fd_pass,I_SENDFD,fd) ;
	    } else {
		rs = SR_NOSTR ;
	    }
	} /* end if (u_fstat) */
	return rs ;
} /* end subroutine (uc_fpassfd) */


