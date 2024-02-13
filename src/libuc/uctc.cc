/* uctc SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<termios.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<cstdlib>
#include	<usystem.h>		/* |msleep(3u)| */
#include	<usupport.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOMEM	(5*60)


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_tcdrain(int fd) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		f_exit = false ;
	repeat {
	    if ((rs = tcdrain(fd)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
		    if (to_nomem-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (uc_tcdrain) */


uc_tcdrain.c
uc_tcflow.c
uc_tcflush.c
uc_tcgetattr.c
uc_tcgetpgrp.c
uc_tcgetsid.c
uc_tcsetattr.c
uc_tcsetpgrp.c
