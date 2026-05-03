/* ucfstat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* get the size of an open file */
/* version %I% last-modified %G% */


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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"ucfstat.h"

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_fstat(int fd,ustat *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    if ((rs = u_fstat(fd,sbp)) >= 0) {
	        csize fsize = size_t(sbp->st_size) ;
	        rs = intsat(fsize) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_fstat) */

int uc_fsize(int fd) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    csize fsize = size_t(sb.st_size) ;
	    rs = intsat(fsize) ;
	}
	return rs ;
}
/* end subroutine (uc_fsize) */

int uc_fuid(int fd) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    rs = intsat(sb.st_uid) ;
	}
	return rs ;
}
/* end subroutine (uc_fuid) */

int uc_fgid(int fd) noex {
	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    rs = intsat(sb.st_gid) ;
	}
	return rs ;
}
/* end subroutine (uc_fgid) */


