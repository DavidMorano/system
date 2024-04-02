/* ucfstat SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* get the size of an open file */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<climits>		/* <- for |INT_MAX| */
#include	<usystem.h>
#include	<intsat.h>


/* exported subroutines */

int uc_fsize(int fd) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    rs = intsat(sb.st_size) ;
	}
	return rs ;
}
/* end subroutine (uc_fsize) */

int uc_fuid(int fd) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    rs = intsat(sb.st_uid) ;
	}
	return rs ;
}
/* end subroutine (uc_fuid) */

int uc_fgid(int fd) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    rs = intsat(sb.st_gid) ;
	}
	return rs ;
}
/* end subroutine (uc_fgid) */


