/* ucsetappend */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set the APPEND file descriptor open-flag */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_setappend(int fd,int f) noex {
	int		rs ;
	int		pfl = false ;
	if ((rs = u_fcntl(fd,F_GETFL,0)) >= 0) {
	    int	flflags = rs ;
	    pfl = (flflags & O_APPEND) ? 1 : 0 ;
	    if (! LEQUIV(pfl,f)) {
	        if (f) {
	            flflags |= O_APPEND ;
	        } else {
	            flflags &= (~ O_APPEND) ;
		}
	        rs = u_fcntl(fd,F_SETFL,flflags) ;
	    } /* end if (needed a change) */
	} /* end if (u_fcntl) */
	return (rs >= 0) ? pfl : rs ;
}
/* end subroutine (uc_setappend) */

int uc_append(int fd,int f) noex {
	return uc_setappend(fd,f) ;
}
/* end subroutine (uc_append) */


