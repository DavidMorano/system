/* usupport */
/* lang=C++20 */

/* UNIX-kernal support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This was first written to give a little bit to UNIX® what we have in our
        own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	msleep

	Description:
	This subroutine sleeps for some number of milliseconds.

	Synopsis:
	int msleep(int msec) noex ;

	Arguments:
	msec		number of millisecond to sleep

	Returns:
	>=0		amount of data returned
	<0		error


	Name:
	mtime

	Description:
	This is sort of like |time(2)| but returns milliseconds
	rather than seconds.  Unlike |time(2)|, this subroutine
	takes no arguments.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<poll.h>
#include	<climits>
#include	<ctime>
#include	<cstring>		/* <- for |memset(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"usupport.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

extern "C" int msleep(int msec) noex {
	int		rs = SR_INVALID ;
	if (msec > 0) {
	    pollfd	fds[1] ;
	    cint	irs = SR_INTR ;
	    fds[0].fd = -1 ;
	    fds[0].events = 0 ;
	    if ((rs = u_poll(fds,0,msec)) == irs) {
		rs = 1 ;
	    }
	} else if (msec == 0) {
	    rs = SR_OK ;
	} /* end if (valiid argument) */
	return rs ;
}
/* end subroutine (msleep) */

extern "C" mtime_t mtime(void) noex {
	struct timeval	tv ;
	mtime_t		t ;
	mtime_t		m = 0 ;
	if (gettimeofday(&tv,nullptr) >= 0) {
	    t = tv.tv_sec ;
	    m += (t*1000) ;
	    m += (tv.tv_usec / 1000) ;
	} else {
	    t = time(nullptr) ;
	    m += (t*1000) ;
	}
	return m ;
}
/* end subroutine (mtime) */

extern "C" int memclear(void *vp,size_t sz) noex {
	int		rs = SR_FAULT ;
	if (vp) {
	    rs = int(sz & INT_MAX) ;
	    memset(vp,0,sz) ;
	}
	return rs ;
}
/* end subroutine (memclear) */


