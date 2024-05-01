/* uc_realpath SUPPORT */
/* lang=C++20 */

/* resolve a path without symbolic or relative components */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine takes an existing path and creates a new path
	that does not contain either symbol or relative components.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cerrno>
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* local variables */


/* exported variables */


/* external subroutines */

int uc_realpath(cchar *fname,char *rbuf) noex {
	int		rs = SR_FAULT ;
	if (fname && rbuf) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (realpath(fname,rbuf) == nullptr) {
		    rs = (- errno) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_realpath) */


