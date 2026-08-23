/* ucnonblock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* set the NONBLOCK file descriptor open-flag */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucnonblock.h"

/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_nonblock(int fd,int f) noex {
	int		rs ;
	int		fprev = false ;
	if ((rs = u_fcntl(fd,F_GETFL,0)) >= 0) {
	    int		fl = rs ;
	    fprev = (fl & O_NONBLOCK) ? 1 : 0 ;
	    if (! LEQUIV(fprev,f)) {
	        if (f) {
	            fl |= O_NONBLOCK ;
	        } else {
	            fl &= (compl O_NONBLOCK) ;
		}
	        rs = u_fcntl(fd,F_SETFL,fl) ;
	    } /* end if (needed a change) */
	} /* end if (u_fcntl) */
	return (rs >= 0) ? fprev : rs ;
} /* end subroutine (uc_nonblock) */


