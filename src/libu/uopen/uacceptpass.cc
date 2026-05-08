/* uacceptpass SUPPORT */
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
#include	<sys/conf.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<ctime>			/* |time(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usyscalls.h>
#include	<localmisc.h>		/* |POLL_INTMULT| */


/* local defines */

#ifndef STRRECVFD
#define	STRRECVFD	struct strrecvfd
#endif

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif

#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
#define	F_STREAMS	1
#else
#define	F_STREAMS	0
#endif


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

static int acceptpass_stall(int,STRRECVFD *) noex ;
static int acceptpass_poll(int,STRRECVFD *,int) noex ;


/* local variables */

constexpr bool		f_streams = F_STREAMS ;


/* exported variables */


/* exported subroutines */

namespace libu {
    int special_acceptpass(int fd_pass,STRRECVFD *sp,int to) noex {
	int		rs = SR_BADF ;
	if (fd_pass >= 0) {
	    rs = SR_NOSYS ;
	    if_constexpr (f_streams) {
	        STRRECVFD	dummy ;
	        if (sp == nullptr) sp = &dummy ;
	        if (to >= 0) {
	            rs = acceptpass_poll(fd_pass,sp,to) ;
	        } else {
	            rs = acceptpass_stall(fd_pass,sp) ;
	        }
	    } /* enf if_constexpr (f_streams) */
	} /* end if (valid) */
	return rs ;
    } /* end subroutine (acceptpass) */
} /* end namespace (libu) */


/* local subroutines */

static int acceptpass_poll(int fd_pass,STRRECVFD *sp,int to) noex {
	POLLFD		pfds[2] = {} ;
	time_t		ti_now = time(nullptr) ;
	time_t		ti_start ;
	int		rs ;
	int		nfds = 0 ;
	int		pfd = -1 ; /* return-value */
	bool		f_timed = (to > 0) ;
	{
	    pfds[nfds].fd = fd_pass ;
	    pfds[nfds].events = (POLLIN | POLLPRI) ;
	    nfds += 1 ;
	}
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
	int		pfd = -1 ; /* return-value */
	while (rs != SR_OK) {
	    rs = u_ioctl(fd_pass,I_RECVFD,sp) ;
	    pfd = sp->fd ;
	    if ((rs == SR_BADMSG) || (rs == SR_INTR)) {
		rs = SR_OK ;
	    }
	} /* end while */
	return (rs >= 0) ? pfd : rs ;
}
/* end subroutine (acceptpass_stall) */


