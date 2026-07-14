/* debsetfd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* set debug file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* exported variables */

int	debug_fd = -1 ;


/* exported subroutines */

int debsetfd(int fd) noex {
	int		rs = SR_BADF ;
	if (fd < 256) {
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
		err_fd = fd ;
		rs = fd ;
	    } else {
		rs = (- errno) ;
	    }
	}
	return rs ;
} /* end subroutine (debsetfd) */


