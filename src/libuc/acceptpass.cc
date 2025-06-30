/* acceptpass SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to accept an FD that was passed through a FIFO */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	acceptpass

	Description:
	This little subroutine accepts a file descriptor from some
	type of FIFO, pipe, or other (weirdo) STREAMS-like thing.
	This subroutine will not compile on a non-STREAMS system;
	we depend on passing FDs through pipes and not eveyone
	(every OS) has that ability.

	Synopsis:
	int acceptpass(int fd_pass,STRRECVFD *sp,int to) noex

	Arguments:
	fd_pass		FIFO file descriptor
	sp		pointer to structure to receive the passed FD
	to		time-out in seconds

	Returns:
	>=0		reeived file descriptot
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>			/* |time(2)| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"acceptpass.h"


/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

static int acceptpass_stall(int,STRRECVFD *) noex ;
static int acceptpass_poll(int,STRRECVFD *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int acceptpass(int fd_pass,STRRECVFD *sp,int to) noex {
	STRRECVFD	dummy ;
	int		rs = SR_BADF ;
	if (sp == nullptr) sp = &dummy ;
	if (fd_pass >= 0) {
	    if (to >= 0) {
	        rs = acceptpass_poll(fd_pass,sp,to) ;
	    } else {
	        rs = acceptpass_stall(fd_pass,sp) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (acceptpass) */


/* local subroutines */

static int acceptpass_poll(int fd_pass,STRRECVFD *sp,int to) noex {
	POLLFD		pfds[2] = {} ;
	time_t		ti_now = time(nullptr) ;
	time_t		ti_start ;
	int		rs ;
	int		nfds = 0 ;
	int		pfd = -1 ;
	bool		f_timed = (to > 0) ;
	pfds[nfds].fd = fd_pass ;
	pfds[nfds].events = (POLLIN | POLLPRI) ;
	nfds += 1 ;
	ti_start = ti_now ;
	while ((rs = u_poll(pfds,nfds,POLL_INTMULT)) >= 0) {
	    ti_now = time(nullptr) ;
	    if (rs > 0) {
		for (int i = 0 ; (rs >= 0) && (i < nfds) ; i += 1) {
		    cint	fd = pfds[i].fd ;
		    cint	re = pfds[i].revents ;
		    if (fd == fd_pass) {
		   	if ((re & POLLIN) || (re & POLLPRI)) {
			    rs = u_ioctl(fd_pass,I_RECVFD,sp) ;
			    pfd = sp->fd ;
			    if (rs > 0) break ;
			    if ((rs == SR_BADMSG) || (rs == SR_INTR)) {
				rs = SR_OK ;
			    }
			} /* end if (event) */
		    } /* end if (ours) */
		} /* end for */
	    } else if (rs == SR_INTR) {
		rs = SR_OK ;
	    }
	    if ((rs >= 0) && (pfd >= 0)) break ;
	    if ((rs >= 0) && f_timed && ((ti_now - ti_start) >= to)) {
		rs = SR_TIMEDOUT ;
	    }
	    if (rs < 0) break ;
	} /* end while (polling) */
	return (rs >= 0) ? pfd : rs ;
}
/* end subroutine (acceptpass_poll) */

static int acceptpass_stall(int fd_pass,STRRECVFD *sp) noex {
	int		rs = SR_OK ;
	int		pfd = -1 ;
	while (rs >= 0) {
	    rs = u_ioctl(fd_pass,I_RECVFD,sp) ;
	    pfd = sp->fd ;
	    if (rs >= 0) break ;
	    if ((rs == SR_BADMSG) || (rs == SR_INTR)) {
		rs = SR_OK ;
	    }
	} /* end while */
	return (rs >= 0) ? pfd : rs ;
}
/* end subroutine (acceptpass_stall) */


