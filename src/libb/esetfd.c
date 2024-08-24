/* debsetfd SUPPORT */
/* lang=C++20 */

/* set debug file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


/* local defines */


/* exported variables */

int	debug_fd = -1 ;


/* exported subroutines */

int debsetfd(int fd) noex {
	USTAT		sb ;
	int		rs = SR_BADF ;
	if (fd < 256) {
	    if ((rs = fstat(fd,&sb)) >= 0) {
		err_fd = fd ;
		rs = fd ;
	    } else {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (debsetfd) */


