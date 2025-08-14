/* dialpass SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to dial over to a UNIX® domain socket in stream mode */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialpass

	Description:
	This subroutine will dial out to an UNIX® domain socket
	stream address.

	Synopsis:
	int dialpass(cchar *fname,int timeout,int opts) noex

	Arguments:
	fname		path to UNIX® domain socket to dial to
	timeout		timeout ('>=0' mean use one, '-1' means do not)
	opts		any dial opts

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/poll.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"dialpass.h"


/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif

#ifndef	I_SENDFD
#define	I_SENDFD	0
#endif

#ifndef	SYSHAS_STREAMS
#define	SYSHAS_STREAMS	0
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	waitready(int,int) noex ;


/* local variables */

constexpr bool	f_streams = SYSHAS_STREAMS ;


/* exported variables */


/* exported subroutines */

int dialpass(cchar *fname,int timeout,int) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		rs = SR_NOSYS ;
		if_constexpr (f_streams) {
	            cint	of = (O_WRONLY | O_NDELAY) ;
		    cmode	om = 0666 ;
	            if ((rs = uc_open(fname,of,om)) >= 0) {
	                cint	fd_pass = rs ;
	                if (USTAT sb ; (rs = uc_fstat(fd_pass,&sb)) >= 0) {
			    rs = SR_INVALID ;
	                    if (S_ISFIFO(sb.st_mode) || S_ISCHR(sb.st_mode)) {
			        rs = SR_OK ;
	                        if (timeout >= 0) {
	                            rs = waitready(fd_pass,timeout) ;
	                        } /* end if (timeout) */
	                        if (rs >= 0) {
	                            int		pipes[2] ;
	                            if ((rs = uc_pipe(pipes)) >= 0) {
					cint	cmd = I_SENDFD ;
	                                fd = pipes[0] ;
	                                rs = uc_ioctl(fd_pass,cmd,pipes[1]) ;
	                                uc_close(pipes[1]) ;
	                                if (rs < 0) {
	                                    uc_close(fd) ;
					    fd = -1 ;
	                                }
	                            } /* end if (uc_pipe) */
	                        } /* end if (ok) */
			    } /* end if (type of file) */
	                } /* end if (stat) */
	                rs1 = uc_close(fd_pass) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (uc_open) */
		    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
		} /* end if_constexpr (f_streams) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialpass) */


/* local subroutines */

static int waitready(int fd,int timeout) noex {
	time_t		ti_timeout ;
	time_t		dt = time(nullptr) ;
	int		rs = SR_OK ;
	int		f = FALSE ;
	if (timeout >= 0) {
	    POLLFD	polls[2] = {} ;
	    polls[0].fd = fd ;
	    polls[0].events = (POLLOUT | POLLWRBAND) ;
	    polls[1].fd = -1 ;
	    polls[1].events = 0 ;
	    ti_timeout = dt + timeout ;
	    while (rs >= 0) {
	        cint	pollto = MIN(timeout,5) * POLL_INTMULT ;
	        if ((rs = u_poll(polls,1,pollto)) > 0) {
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
	        if ((rs >= 0) && (! f)) {
	            dt = time(nullptr) ;
		    if (dt >= ti_timeout) rs = SR_TIMEDOUT ;
	        }
	        if ((rs >= 0) && f) break ;
	    } /* end while */
	} /* end if (ok) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (waitready) */


