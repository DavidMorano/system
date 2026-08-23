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
#include	<sys/types.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

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
} /* end subroutine (uc_ndelay) */


