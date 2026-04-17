/* ucsettimeofday */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* set the current time of day */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This routine calls the system's (library) 'settimeofday'
	subroutine.

	Question:
		What does this stupid function return?
		The documentation is not as clear as it could be!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/time.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<localmisc.h>


/* local variables */


/* exported variables */


/* external subroutines */

int uc_settimeofday(TIMEVAL *tvp,cvoid *cvp) noex {
	int		rs = SR_FAULT ;
	if (tvp) {
	    rs = SR_OK ;
	    {
	        auto *ctp = (CTIMEZONE *) cvp ;
	        rs = settimeofday(tvp,ctp) ;
	    } /* end block */
	    if (rs < 0) {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_settimeofday) */


