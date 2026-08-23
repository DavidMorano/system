/* ucsockrecve SUPPORT */
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
	uc_recve

	Description:
	Get some amount of data, and time it also so that we can
	abort if it times out.

	Synopsis:
	int uc_recve(int fd,char *rbuf,int rlen,int mflags,int to,int opts) noex

	Arguments:
	fd		file descriptor
	rbuf		user buffer to receive daa
	rlen		maximum amount of data the user wants
	mflags		option flags for MSG reception
	to		time in seconds to wait
	opts		options

	Returns:
	>=0		amount of data returned
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<sys/uio.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucdesc.h>		/* LIBUC */
#include	<bufprintf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"ucdescsock.h"


/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll() takes milliseconds ! */
#endif

#ifndef	POLLTIMEINT
#define	POLLTIMEINT	10		/* seconds */
#endif

#define	EBUFLEN		100

#define	POLLEVENTS	(POLLIN | POLLPRI) ;

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug printo-outs */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

#if	CF_DEBUG
static char	*d_reventstr() ;
#endif


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int uc_recve(int fd,void *rbuf,int rlen,int mflags,int to,int opts) noex {
	POLLFD		fds[2] = {} ;
	time_t		tiprev = time(nullptr) ;
	time_t		tinow ;
	int		rs = SR_OK ;
	int		events = POLLEVENTS ;
	int		pollint ;
	int		tlen = 0 ;
	int		f_eof = FALSE ;

#if	CF_DEBUG
	char	ebuf[EBUFLEN + 1] ;
#endif

#if	CF_DEBUG
	DEBUGPRINTF("uc_recve: ent rlen=%u to=%d opts=%08x\n",
		rlen,to,opts) ;
#endif

	if (rlen <= 0)
	    return SR_OK ;

	if (to < 0) to = INT_MAX ;

#ifdef	POLLRDNORM
	events |= POLLRDNORM ;
#endif
#ifdef	POLLRDBAND
	events |= POLLRDBAND ;
#endif

	pollint = MIN(to,POLLTIMEINT) ;

	fds[0].fd = fd ;
	fds[0].events = short(events) ;
	fds[1].fd = -1 ;
	fds[1].events = 0 ;

	while ((rs >= 0) && (to >= 0)) {

#if	CF_DEBUG
	    DEBUGPRINTF("uc_recve: u_poll() pollint=%d to=%d\n",
	        pollint,to) ;
#endif

	    if ((rs = u_poll(fds,1,(pollint * POLL_INTMULT))) > 0) {

#if	CF_DEBUG
	        DEBUGPRINTF("uc_recve: events %s\n",
	            d_reventstr(fds[0].revents,ebuf,EBUFLEN)) ;

	        DEBUGPRINTF("uc_recve: about to 'read'\n") ;
#endif

	        rs = u_recv(fd,rbuf,rlen,mflags) ;
	        tlen = rs ;

#if	CF_DEBUG
	        DEBUGPRINTF("uc_recve: u_read() rs=%d\n",
	            rs) ;
#endif

		f_eof = (tlen == 0) ;
	        break ;

	    } else if (rs == 0) {

		if (to > 0) {
	            tinow = time(nullptr) ;
	            to -= intconv((tinow - tiprev)) ;
	            tiprev = tinow ;
	            if (to < POLLTIMEINT) {
	                pollint = to ;
		    }

		} else
	            break ;

	    } else {
		if (rs == SR_INTR) rs = SR_OK ;
	    }

	} /* end while */

	if ((rs >= 0) && (tlen == 0) && (to <= 0) && 
	    (! f_eof) && (rlen > 0)) {

	    if (opts & FM_AGAIN) {
		rs = SR_AGAIN ;
	    } else if (opts & FM_TIMED) {
		rs = SR_TIMEDOUT ;
	    }

	} /* end if */

#if	CF_DEBUG
	DEBUGPRINTF("uc_recve: ret rs=%d tlen=%u\n",
	    rs,tlen) ;
#endif

	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (uc_recve) */

#if	CF_DEBUG
static char *d_reventstr(int revents,char *rbuf,int rlen) noex {
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


