/* u_execv SUPPORT */
/* version %I% last-modified %G% */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_AGAIN	(1 * 60)	/* fairly long! */
#define	TO_NOMEM	(1 * 60)	/* fairly long! */
#define	TO_TXTBUSY	(1 * 60)	/* fairly long! */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int u_execv(cchar *p,mainv argv) noex {
	char *const	*eav = (char *const *) argv ;
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_txtbusy = utimeout[uto_txtbusy] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = execv(p,eav)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_NOMEM:
	            if (to_nomem -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_TXTBUSY:
	            if (to_txtbusy -- > 0) {
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
/* end subroutine (u_execv) */


