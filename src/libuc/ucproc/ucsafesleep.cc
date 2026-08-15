/* ucsafesleep SUPPORT **/
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* safely sleep for a while */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written so that we could use a single
	directory interface due to all of the changes in the POSIX
	standard and previous UNIX® standards.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_safesleep

	Description:
	This code safely sleeps for a while without interferring
	with the dangerous and fragile ALARM signal (which likely
	gets changed to different things under some execution modes
	-- like with mutlithreading on Slowlaris!).  The time
	precision is one second.

	Synopsis:
	int uc_safesleep(int n) noex

	Arguments:
	n	number of seconds to sleep for

	Returns:
	>=0	OK
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<getusmtime.hh>		/* LIBU |getusmtime(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"ucsafesleep.h"

/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward referecnces */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_safesleep(int n) noex {
	mtime_t		dt = getusmtime ;
	int		rs = SR_OK ;
	if (n > 0) {
	    POLLFD	fds[1] = {} ;
	    mtime_t	mn = mtime_t(n * POLL_INTMULT) ;
	    mtime_t	st = dt ;
	    fds[0].fd = -1 ;
	    fds[0].events = 0 ;
	    fds[0].revents = 0 ;
	    while ((rs >= 0) && ((dt - st) < mn)) {
	        cint	intpoll = int(mn - (dt - st)) ;
	        rs = u_poll(fds,0,intpoll) ;
	        dt = getusmtime ;
		if (rs == SR_INTR) rs = SR_OK ;
	    } /* end while */
	} /* end if (positive) */
	return rs ;
} /* end subroutine (uc_safesleep) */


