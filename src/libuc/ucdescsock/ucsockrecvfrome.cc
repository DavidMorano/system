/* ucsockrecvfrome SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* extended read */

#define	CF_DEBUG	0		/* non-switchable debug printo-outs */

/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_sockrecvfrome

	Description:
	Get some amount of data and time it also so that we can
	abort if it times out.

	Synopsis:
	int uc_recvfrome(int fd,void *rbuf,int rlen,int flags,
		SICKADDR *fromp,int *fromlenp,
		timeout,opts) noex

	Arguments:
	fd		file descriptor
	rbuf		user buffer to receive daa
	rlen		maximum amount of data the user wants
	flags		option flags for MSG reception
	fromp		pointer to socket address structure
	fromlenp	pointer to length of socket address
	timeout		time in seconds to wait
	opts		time in seconds to wait

	Returns:
	>=0		amount of data returned
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<sys/uio.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucdescsock.h>		/* LIBUC */
#include	<bufprintf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */


/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll() takes milliseconds ! */
#endif

#define	TI_POLL		10
#define	EBUFLEN		100

#define	POLLEVENTS	(POLLIN | POLLPRI)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug printo-outs */
#endif


/* external subroutines */


/* forward references */

#if	CF_DEBUG
local char	*d_reventstr() ;
#endif


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int uc_recvfrome(int fd,void *rbuf,int rlen,int flags,
		void *fromvp,int *fromlenp,int timeout,int opts) noex {
	POLLFD		fds[2] = {} ;
	SOCKADDR	*fromp = (SOCKADDR *) fromvp ;
	time_t	previous = time(nullptr) ;
	time_t	current ;
	int	rs = SR_OK ;
	int	pollint ;
	int	len = 0 ;
	int	f_first ;
	short events = POLLEVENTS ;

#if	CF_DEBUG
	char	ebuf[EBUFLEN + 1] ;
#endif

	    DPRINTF("uc_recvfrome: rlen=%d\n",rlen) ;
	    DPRINTF("uc_recvfrome: flags=%04x\n",flags) ;
	    DPRINTF("uc_recvfrome: timeout=%d\n",timeout) ;

	(void) opts ;
	if (rlen <= 0) return SR_OK ;

	if (rbuf == nullptr) return SR_FAULT ;

	if (timeout < 0) timeout = INT_MAX ;

	pollint = (timeout > TI_POLL) ? TI_POLL : timeout ;

#ifdef	POLLRDNORM
	events |= POLLRDNORM ;
#endif
#ifdef	POLLRDBAND
	events |= POLLRDBAND ;
#endif

	fds[0].fd = fd ;
	fds[0].events = events ;
	fds[1].fd = -1 ;

	f_first = true ;
	while (f_first || (timeout > 0)) {

	    f_first = false ;
	    rs = u_poll(fds,1,(pollint * POLL_INTMULT)) ;
	    if (rs < 0) break ;
	    DPRINTF("back from POLL w/ rs=%d\n", rs) ;
	    if (rs > 0) {
#if	CF_DEBUG
	        DPRINTF("events %s\n",
	            d_reventstr(fds[0].revents,ebuf,EBUFLEN)) ;
#endif
	        DPRINTF("about to 'read'\n") ;
	        rs = u_recvfrom(fd,rbuf,rlen,flags,fromp,fromlenp) ;
	        len = rs ;
	        DPRINTF("u_recvfrom() rs=%d\n", rs) ;
	        break ;

	    } else {
	        current = time(nullptr) ;
	        timeout -= intconv(current - previous) ;
	        previous = current ;
	        if (timeout < TI_POLL) {
	            pollint = timeout ;
		}
	        if (timeout <= 0) {
	            rs = SR_TIMEDOUT ;
		}
	    } /* end if */
	} /* end while */
	DPRINTF("ret rs=%d len=%d\n", rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (uc_recvfrome) */


/* local subroutines */

#if	CF_DEBUG
local char *d_reventstr(int revents,char *rbuf,int rlen) noex {
	rbuf[0] = '\0' ;
	bufprintf(rbuf,rlen,"%s %s %s %s %s %s %s %s %s",
	    (revents & POLLIN) ? "I " : "  ",
	    (revents & POLLRDNORM) ? "IN" : "  ",
	    (revents & POLLRDBAND) ? "IB" : "  ",
	    (revents & POLLPRI) ? "PR" : "  ",
	    (revents & POLLWRNORM) ? "WN" : "  ",
	    (revents & POLLWRBAND) ? "WB" : "  ",
	    (revents & POLLERR) ? "ER" : "  ",
	    (revents & POLLHUP) ? "HU" : "  ",
	    (revents & POLLNVAL) ? "NV" : "  ") ;
	return rbuf ;
} /* end subroutine (d_reventstr) */
#endif /* CF_DEBUG */


