/* ucgetrusage SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* retrieve process usage information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_getrusage

	Description:
	This subroutine retrieves process usage data from the kernel.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/resource.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getrusage(int who,RUSAGE *rp) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = getrusage(who,rp)) < 0) {
		rs = (- errno) ;
	    }
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
		    if (to_again-- > 0) {
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
/* end subroutine (uc_getrusage) */


