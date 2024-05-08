/* time SUPPORT */
/* lang=C++20 */

/* this is the intercept for the UNIX® System |time(2)| call */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a version of |time(2)| that is preloaded to over-ride the
	standard UNIX® system version.

	Q. Is this multi-thread safe?
	A. Since it is a knock-off of an existing UNIX® system LIBC (3c)
	   subroutine that is already multi-thread safe -- then of course
	   it is!

	Q. Is this much slower than the default system version?
	A. No, not really.

	Q. How are we smarter than the default system version?
	A. Let me count the ways:
		+ value is cached!

	Q. Why did you not also interpose something for |sysinfo(2)|?
	A. Because we did not want to.

	Q. Why are you so smart?
	A. I do not know.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"preload.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

time_t time(time_t *tp) noex {
	time_t		t = 0 ;
	int		rs ;
	if ((rs = pretime_modtime(&t)) < 0) {
	    ulogerror("time",rs,"pretime_modtime") ;
	}
	if (tp) {
	    *tp = (rs >= 0) ? t : 0 ;
	}
	return t ;
}
/* end subroutine (time) */


