/* ucrecvmsge SUPPORT */
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

	Get some amount of data and time it also so that we can
	abort if it times out.

	Synopsis:
	int uc_recvmsge(int fd,MSGHDRD *msgp,int flags,
		int timeout,int opts) noex

	Arguments:
	fd		file descriptor
	msgp		pointer to MSG structure
	flags		option flags for the reception of MSG
	timeout		time in seconds to wait
	opts		options

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
#include	<usupport.h>		/* LIBU */
#include	<bufprintf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucrecvmsge.h"


/* local defines */

#define	POLL_INTMULT	1000		/* poll() takes milliseconds! */
#define	TO_POLL		10
#define	EBUFLEN		100

#define	POLLEVENTS	(POLLIN | POLLPRI)


/* external subroutines */


/* forward references */

#if	CF_DEBUG
local char	*d_reventstr() ;
#endif


/* exported variables */


/* exported subroutines */

int uc_recvmsge(int fd,MSGHDR *msgp,int flags,int timeout,int opts) noex {
	POLLFD		fds[2] = {} ;
	time_t		previous = time(nullptr) ;
	time_t		current ;
	int		rs = SR_OK ;
	int		nfds ;
	int		to = timeout ;
	short		events = POLLEVENTS ;
	int		pollint ;
	int		len = 0 ;
	int		f_first = true ;

#if	CF_DEBUG
	char		ebuf[EBUFLEN + 1] ;
#endif

	(void) opts ;
	if (msgp == nullptr) return SR_FAULT ;

	if (timeout < 0) {
	    timeout = INT_MAX ;
	}

	pollint = TO_POLL ;
	if (timeout >= 0) {
	    pollint = MIN(timeout,TO_POLL) ;
	}

#if	defined(POLLRDNORM)
	events |= POLLRDNORM ;
#endif
#if	defined(POLLRDBAND)
	events |= POLLRDBAND ;
#endif

	nfds = 0 ;
	fds[nfds].fd = fd ;
	fds[nfds].events = events ;
	nfds += 1 ;
	fds[nfds].fd = -1 ;

	while ((rs >= 0) && (f_first || (to > 0))) {
	    int	f_break = false ;
	    f_first = false ;

	    if ((rs = u_poll(fds,nfds,(pollint * POLL_INTMULT))) > 0) {
		cint	re = fds[0].revents ;
		if ((re & POLLIN) || (re & POLLPRI)) {
	            rs = u_recvmsg(fd,msgp,flags) ;
	            len = rs ;
		    f_break = true ;
		} else if (re & POLLNVAL) {
		    rs = SR_NOTOPEN ;
		} else if (re & POLLERR) {
		    rs = SR_POLLERR ;
		} else if (re & POLLHUP) {
		    msleep(1) ;
		} /* end if */
	    } else if (rs == 0) {
	        current = time(nullptr) ;
	        to -= intconv(current - previous) ;
	        previous = current ;
	        if (to < TO_POLL) {
	            pollint = to ;
		}
	        if (to <= 0) {
	            rs = SR_TIMEDOUT ;
		}
	    } else if (rs == SR_INTR) {
		rs = SR_OK ;
	    }
	    if (f_break) break ;
	} /* end while */

	if ((rs >= 0) && (timeout >= 0) && (to <= 0)) {
	    rs = SR_TIMEDOUT ;
	}

#if	CF_DEBUG
	debugprintf("uc_recvmsge: ret rs=%d len=%d\n",
	    rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
} /* end subroutine (uc_recvmsge) */

#if	CF_DEBUG
local char *d_reventstr(int revents,char *buf,int buflen) noex {
	buf[0] = '\0' ;
	bufprintf(buf,buflen,"%s %s %s %s %s %s %s %s %s",
	    (revents & POLLIN) ? "I " : "  ",
	    (revents & POLLRDNORM) ? "IN" : "  ",
	    (revents & POLLRDBAND) ? "IB" : "  ",
	    (revents & POLLPRI) ? "PR" : "  ",
	    (revents & POLLWRNORM) ? "WN" : "  ",
	    (revents & POLLWRBAND) ? "WB" : "  ",
	    (revents & POLLERR) ? "ER" : "  ",
	    (revents & POLLHUP) ? "HU" : "  ",
	    (revents & POLLNVAL) ? "NV" : "  ") ;
	return buf ;
} /* end subroutine (d_reventstr) */
#endif /* CF_DEBUG */


