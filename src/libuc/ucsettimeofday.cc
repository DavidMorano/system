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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/time.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


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
		rs = (neg errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uc_settimeofday) */


