/* u_tell SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<intsat.h>
#include	<localmisc.h>


/* local defines */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_tell(int fd,off_t *rp) noex {
	off_t		ro ;
	int		rs ;
	repeat {
	    rs = SR_OK ;
	    if ((ro = lseek(fd,0L,SEEK_CUR)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	if (rs >= 0) rs = intsat(ro) ;
	if (rp) {
	    *rp = (rs >= 0) ? ro : 0 ;
	}
	return rs ;
}
/* end subroutine (u_tell) */


