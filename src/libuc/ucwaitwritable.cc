/* uc_waitwritable SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* wait for an FD to become writable */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	Wait for an FD to become writable.

	Description:
	Wait for a file-descriptor to become writable.

	Synopsis:
	int uc_waitwritable(int fd,int to) noex

	Arguments:
	fd		FD to wait on
	to 		time-out in seconds to wait for

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/poll.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	BUFLEN
#define	BUFLEN		(8 * 1024)
#endif

#define	INTPOLL		10		/* seconds */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_waitwritable(int fd,int timeout) noex {
	POLLFD		polls[2] ;
	time_t		dt = 0 ;
	time_t		ti_timeout = 0 ;
	int		rs = SR_OK ;
	int		pollto = (INTPOLL * POLL_INTMULT) ;
	int		nfds = 0 ;
	int		f = FALSE ;
	if (timeout >= 0) {
	    dt = time(nullptr) ;
	    ti_timeout = (dt + timeout) ;
	    pollto = min(timeout,5) * POLL_INTMULT ;
	}
	polls[nfds].fd = fd ;
	polls[nfds].events = (POLLOUT | POLLWRBAND) ;
	polls[nfds].revents = 0 ;
	nfds += 1 ;
	while (rs >= 0) {
	    if ((rs = u_poll(polls,nfds,pollto)) > 0) {
		cint	re = polls[0].revents ;
	        if (re & POLLHUP) {
	            rs = SR_HANGUP ;
	        } else if (re & POLLERR) {
	            rs = SR_POLLERR ;
	        } else if (re & POLLNVAL) {
	            rs = SR_BADF ;
	        } else if ((re & POLLOUT) || (re & POLLWRBAND)) {
		    f = TRUE ;
		}
	    } else if (rs == SR_INTR) {
		rs = SR_OK ;
	    }
	    if ((rs >= 0) && (! f) && (timeout >= 0)) {
	        dt = time(nullptr) ;
		if (dt >= ti_timeout) rs = SR_TIMEDOUT ;
	    }
	    if ((rs >= 0) && f) break ;
	} /* end while */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uc_waitwritable) */


