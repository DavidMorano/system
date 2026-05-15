/* ucndelay SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* set the NDELAY file descriptor open-flag */


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
#include	<ucdesc.h>
#include	<localmisc.h>

#include	"ucndelay.h"

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

int uc_ndelay(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = u_fcntl(fd,F_GETFL,0)) >= 0) {
	    int		flflags = rs ;
	    f_previous = (flflags & O_NDELAY) ? 1 : 0 ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            flflags |= O_NDELAY ;
	        } else {
	            flflags &= (~ O_NDELAY) ;
		}
	        rs = u_fcntl(fd,F_SETFL,flflags) ;
	    } /* end if (needed a change) */
	} /* end if (u_fcntl) */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (uc_ndelay) */


