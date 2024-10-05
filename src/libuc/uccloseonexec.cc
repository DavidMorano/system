/* uccloseonexec SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* set the CLOSE-ON-EXEC file descriptor flag */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>		/* |LEQUIV(3dam)| */


/* local defines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_closeonexec(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = u_fcntl(fd,F_GETFD,0)) >= 0) {
	    int		fdflags = rs ;
	    f_previous = (fdflags & FD_CLOEXEC) ? 1 : 0 ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            fdflags |= FD_CLOEXEC ;
	        } else {
	            fdflags &= (~ FD_CLOEXEC) ;
		}
	        rs = u_fcntl(fd,F_SETFD,fdflags) ;
	    } /* end if (needed a change) */
	} /* end if */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (uc_closeonexec) */


