/* ucfstat SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* get the size of an open file */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<climits>		/* <- for |INT_MAX| */
#include	<usystem.h>


/* exported subroutines */

int uc_fsize(int fd) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    rs = (sb.st_size & INT_MAX) ;
	}
	return rs ;
}
/* end subroutine (uc_fsize) */

int uc_fuid(int fd) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    rs = int(sb.st_uid & INT_MAX) ;
	}
	return rs ;
}
/* end subroutine (uc_fuid) */

int uc_fgid(int fd) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    rs = int(sb.st_gid & INT_MAX) ;
	}
	return rs ;
}
/* end subroutine (uc_fgid) */


