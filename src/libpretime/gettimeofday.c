/* gettimeofday */
/* lang=C20 */

/* this is the intercept for the UNIX® System |gettimeofday(3c)| call */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	gettimeofday

	Description:
	This is a version of |gettimeofday(3c)| that is preloaded
	to over-ride the standard UNIX® system version.

	Q. Is this multi-thread safe?
	A. Since it is a knock-off of an existing UNIX® system 
	   LIBC(3c) subroutine that is already multi-thread safe
	   -- then of course it is!

	Q. Is this much slower than the default system version?
	A. No, not really.

	Q. How are we smarter than the default system version?
	A. Let me count the ways:
		+ value is cached!

	Q. Why did you not also interpose something for |sysinfo(2)|?
	A. Because we did not want to.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>
#include	"pretime.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

/* ARGSUSED */
int gettimeofday(struct timeval *tvp,void *dummy) noex {
	int		rs ;
	if ((rs = pretime_modtv(tvp,dummy)) >= 0) {
	    rs = 0 ;
	} else {
	    errno = (-rs) ;
	    rs = -1 ;
	}
	return rs ;
}
/* end subroutine (gettimeoftime) */


