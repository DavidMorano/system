/* uc_fattach SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<poll.h>
#include	<cerrno>
#include	<usystem.h>


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_fattach(int fd,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if ((rs = fattach(fd,fname)) < 0) {
		    rs = (- errno) ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (uc_fattach) */

int uc_fdetach(cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if ((rs = fdetach(fname)) < 0) {
		    rs = (- errno) ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (uc_fdetach) */


