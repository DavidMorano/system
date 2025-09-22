/* uconnect SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* version %I% last-modified %G% */
/* translation layer interface for UNIX® equivalents */

#define	CF_ISCONN	0		/* change the meaning of ISCONN? */

/* revision history:

	= 1998-02-14, David A-D- Morano
	This code was originally written.

	= 1998-03-26, David A-D- Morano
	This subroutine was updated to handle the case where a file
	descriptor was in non-blocking mode during an initial
	|connect()| operation, was then put into blocking mode, and
	then an additional 'connect()' was issued. Before, we
	returned something like SR_ALREADY rather than SR_OK.  Now,
	we detect if we are now in blocking mode and we will wait
	for the connect to complete.

	= 2017-05-08, David A-D- Morano
	I do not think it is a good idea to ever turn ON the
	compile-time CF_ISCONN switch. That would return SR_OK
	instead of SR_ISCONN when the connection is complete and
	we were in a non-blocking mode.  This would mean that clinets
	would need to poll for either SR_OK or SR_ISCONN if they
	wanted to know if the connection was already completed.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is the friendly hack version of the more
	"standard" UNIX |connect()| subroutine.  I use the term
	"standard" advisedly since the hot-shot hot-heads working
	on BSD UNIX keep chaning the semantics slightly on this
	call and then the System V people change their version
	(usually a while afterwards) to catch up to the new "standard."

	Somewhere along the line, someone suggested to change the
	error return SR_ISCONN to SR_OK when we get it and if we
	are currently in blocking mode.  I put the code in there
	but it is not turned on (see compile flag above 'CF_ISCONN').


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ufiledesc.h"

import libutil ;

/* local defines */

#define	CS		struct connsub
#define	CS_FL		struct connsub_flags

#define	TO_NOBUFS	(5 * 60)		/* seconds */
#define	TO_NOSR		(5 * 60)		/* seconds */
#define	TO_CONNECT	(5 * 60)		/* seconds */
#define	POLLTIMEOUT	(1 * 1000)		/* milliseconds */

#ifndef	CF_ISCONN
#define	CF_ISCONN	0		/* change the meaning of ISCONN? */
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct connsub_flags {
	unsigned int	checkblock:1 ;
	unsigned int	nonblock:1 ;
	unsigned int	ndelay:1 ;
	unsigned int	blocking:1 ;
} ;

struct connsub {
	SOCKADDR	*sap ;
	CS_FL		fl ;
	int		sal ;
	int		fd ;
} ;


/* forward references */

static int	connsub_start(CS *,int,SOCKADDR *,int) noex ;
static int	connsub_finish(CS *) noex ;
static int	connsub_checkblock(CS *) noex ;
static int	connsub_proc(CS *) noex ;
static int	connsub_wait(CS *,int) noex ;


/* local variables */

constexpr bool		f_isconn = CF_ISCONN ;


/* exported variables */


/* exported subroutines */

int u_connect(int s,cvoid *vsap,int sal) noex {
	SOCKADDR 	*sap = (SOCKADDR *) vsap ;
	CS		ci ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = connsub_start(&ci,s,sap,sal)) >= 0) {
	    {
	        rs = connsub_proc(&ci) ;
		rv = rs ;
	    }
	    rs1 = connsub_finish(&ci) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (connsub) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (u_connect) */


/* local subroutines */

static int connsub_start(CS *cip,int fd,SOCKADDR *sap,int sal) noex {
	int		rs = SR_FAULT ;
	if (cip) {
	    rs = memclear(cip) ;
	    cip->fd = fd ;
	    cip->sap = sap ;
	    cip->sal = sal ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (connsub_start) */

static int connsub_finish(CS *cip) noex {
	int		rs = SR_FAULT ;
	if (cip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (connsub_finish) */

static int connsub_checkblock(CS *cip) noex {
	int		rs = SR_OK ;
	if (! cip->fl.checkblock) {
	    cip->fl.checkblock = true ;
	    if ((rs = u_fcntl(cip->fd,F_GETFL,0)) >= 0) {
	        if (rs & O_NONBLOCK) cip->fl.nonblock = true ;
	        if (rs & O_NDELAY) cip->fl.ndelay = true ;
		cip->fl.blocking = ((! cip->fl.nonblock) && (! cip->fl.ndelay)) ;
	    }
	} /* end if (block check) */
	return rs ;
}
/* end subroutine (connsub_checkblock) */

static int connsub_proc(CS *cip) noex {
	cint		sal = cip->sal ;
	cint		s = cip->fd ;
	int		rs ;
	CSOCKADDR	*sap = (SOCKADDR *) cip->sap ;
	int		to_nobufs = utimeout[uto_nobufs] ;
	int		to_nosr = utimeout[uto_nosr] ;
	bool		f_exit = false ;

	repeat {
	    if ((rs = connect(s,sap,sal)) < 0) {
		rs = (- errno) ;
	        switch (rs) {
	        case SR_NOBUFS:
	            if (to_nobufs-- > 0) {
	                msleep(1000) ;
	            } else {
	                f_exit = true ;
	            }
	            break ;
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
	                msleep(1000) ;
	            } else {
	                f_exit = true ;
	            }
	            break ;
	        case SR_INTR:
	            if ((rs = connsub_checkblock(cip)) >= 0) {
	                if (cip->fl.blocking) {
	                    rs = connsub_wait(cip,TO_CONNECT) ;
	                } else {
			    rs = SR_INTR ; /* continue looping */
			}
	            } else {
			f_exit = true ;
		    }
	            break ;
/* a previous connect attempt is still in progress */
	        case SR_ALREADY:
	            if ((rs = connsub_checkblock(cip)) >= 0) {
	                if (cip->fl.blocking) {
	                    rs = connsub_wait(cip,TO_CONNECT) ;
	                } else {
			    rs = SR_ALREADY ; /* propagate */
	            	    f_exit = true ; /* end exit */
			}
		    } else {
			f_exit = true ;
	            }
	            break ;
/* the socket is already connected */
	        case SR_ISCONN:
		    if_constexpr (f_isconn) {
	                if ((rs = connsub_checkblock(cip)) >= 0) {
			    if (cip->fl.blocking) {
			        rs = SR_OK ;
			    } else {
			        rs = SR_ISCONN ; /* propagate */
			        f_exit = true ; /* and exit */
			    }
		        } else {
			    f_exit = true ;
		        }
		    } else {
	                f_exit = true ;
		    } /* end if_constexpr (f_isconn) */
	            break ;
	        default:
	            f_exit = true ;
	            break ;
	        } /* end switch */
	    } /* end if (error) */
	} 
	until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (connsub_proc) */

static int connsub_wait(CS *cip,int to) noex {
	POLLFD		fds[2] = {} ;
	time_t		ti_now = 0 ;
	time_t		ti_start = time(nullptr) ;
	time_t		ti_end ;
	int		rs = SR_OK ;
	int		nfds ;
	bool		f_done = false ;

	ti_end = (ti_start + to) ;

	nfds = 0 ;
	fds[nfds].fd = cip->fd ;
	fds[nfds].events = POLLOUT ;
	fds[nfds].revents = 0 ;
	nfds += 1 ;
	fds[nfds].fd = -1 ;

	while ((rs >= 0) && (ti_now < ti_end)) {

	    if ((rs = u_poll(fds,nfds,POLLTIMEOUT)) > 0) {
	        cint	re = fds[0].revents ;

	        if (re & POLLOUT) {
		    f_done = true ;
	        } else if (re & POLLHUP) {
	            rs = SR_HANGUP ;
	        } else if (re & POLLERR) {
	            rs = SR_POLLERR ;
	        } else if (re & POLLNVAL) {
	            rs = SR_NOTOPEN ;
	        }
	    } else if (rs == SR_INTR) {
	        rs = SR_OK ;
	    } /* end if */

	    if (f_done) break ;
	    ti_now = time(nullptr) ;
	    if (rs < 0) break ;
	} /* end while (loop timeout) */

	if ((rs >= 0) && (ti_now >= ti_end)) {
	    rs = SR_TIMEDOUT ;
	}

	return rs ;
}
/* end subroutine (connsub_wait) */


