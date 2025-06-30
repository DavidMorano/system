/* ucconnecte SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* connection a socket */
/* version %I% last-modified %G% */

#define	CF_BADSOLARIS	0		/* Solaris is bad? */

/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit pack OSes!  The UNIX® system is
	such crap for real-time oriented applications!  Just for
	completeness: Oryx Pecos (a psycho proprietary OS) is not
	the answer either!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_connecte

	Description:
	Here we connect a socket but we do it in a way that we can
	time it out if we need to.  It should have been like this
	from the beginning!

	Synopsis:
	int uc_connecte(int fd,SOCKADDR *sap,int sal,int to) noex

	Arguments:
	fd		file descriptor
	sap		socket address pointer
	sal		socket address length
	to		time in seconds to wait

	Returns:
	>=0		amount of data returned
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<ctime>
#include	<usystem.h>
#include	<usysflag.h>
#include	<localmisc.h>


/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll-time multiplier */
#endif

#define	POLLTIMEOUT	200		/* milliseconds */

#if	(defined(CF_BADSOLARIS) && F_SUNOS)
#define	F_BAD		1
#else
#define	F_BAD		0
#endif

/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	connwait(int,SOCKADDR *,int,int) noex ;


/* local variables */

const bool	f_bad = F_BAD ;


/* exported variables */


/* exported subroutines */

int uc_connecte(int fd,const void *vsap,int sal,int to) noex {
	SOCKADDR	*sap = (SOCKADDR *) vsap ;
	int		rs = SR_OK ;
	int		rs1 ;
	/* set non-blocking mode on this socket */
	if (to >= 0) {
	    if ((rs = uc_nonblock(fd,true)) >= 0) {
	        cint	rsin = SR_INPROGRESS ;
	        cint	f_nonblock = (rs > 0) ;
	        if ((rs = u_connect(fd,sap,sal)) == rsin) {
	             rs = connwait(fd,sap,sal,to) ;
	        } /* end if (type of response from |connect| */
	        rs1 = uc_nonblock(fd,f_nonblock) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (uc_nonblock) */
	} else {
	    rs = u_connect(fd,sap,sal) ;
	} /* end if (time-out or not) */

	return rs ;
}
/* end subroutine (uc_connecte) */


/* local subroutines */

local int connwait(int fd,SOCKADDR *sap,int sal,int to) noex {
	POLLFD		fds[2] = {} ;
	time_t		ti_now = 0 ;
	time_t		ti_start = time(nullptr) ;
	time_t		ti_end ;
	int		rs = SR_OK ;
	int		nfds = 0 ;
	bool		f_done = false ;
	ti_end = ti_start + to ;
	fds[nfds].fd = fd ;
	fds[nfds].events = POLLOUT ;
	fds[nfds].revents = 0 ;
	nfds += 1 ;
	while ((rs >= 0) && (ti_now < ti_end)) {
	    if ((rs = u_poll(fds,nfds,POLLTIMEOUT)) > 0) {
	        cint	re = fds[0].revents ;
	        if (re & POLLOUT) {
		    if_constexpr (f_bad) {
	                if ((rs = u_connect(fd,sap,sal)) >= 0) {
			    f_done = true ;
	                } else if (rs == SR_ISCONN) {
	                    rs = SR_OK ;
			    f_done = true ; /* exit */
		        } else if (rs == SR_ALREADY) {
	                    rs = SR_OK ; /* continue looping */
		        } else {
		            f_done = true ;
		        }
		    } else {
		        f_done = true ;
		    } /* end if_constexpr (f_bad) */
	            break ;
	        } else if (re & POLLHUP) {
	            rs = SR_HANGUP ;
	        } else if (re & POLLERR) {
	            rs = SR_POLLERR ;
	        } else if (re & POLLNVAL) {
	            rs = SR_NOTOPEN ;
		}
	    } else {
	        if (rs == SR_INTR) rs = SR_OK ;
	    }
	    if (f_done) break ;
	    ti_now = time(nullptr) ;
	    if (rs < 0) break ;
	} /* end while (loop timeout) */
	if ((rs >= 0) && (ti_now >= ti_end)) {
	    rs = SR_TIMEDOUT ;
	}
	return rs ;
}
/* end subroutine (connwait) */


