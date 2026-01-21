/* ucfminmod */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set (ensure) a minimum mode on a file */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


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

int uc_fminmod(int fd,mode_t mm) noex {
	int		rs = SR_BADFD ;
	int		fchanged = false ;
	if (fd >= 0) {
	    mm &= (~ S_IFMT) ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	        cmode	cm = (sb.st_mode & (~ S_IFMT)) ;
	        if ((cm & mm) != mm) {
	            fchanged = true ;
	            mm |= cm ;
	            rs = u_fchmod(fd,mm) ;
	        } /* end if (needed a change) */
	    } /* end if (successful stat) */
	} /* end if (valid) */
	return (rs >= 0) ? fchanged : rs ;
}
/* end subroutine (uc_fminmod) */


