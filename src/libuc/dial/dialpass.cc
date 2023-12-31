/* dialpass SUPPORT */
/* lang=C++20 */

/* subroutine to dial over to a UNIX� domain socket in stream mode */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1999 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialpass

	Description:
	This subroutine will dial out to an UNIX� domain socket
	stream address.

	Synopsis:
	int dialpass(cchar *fname,int timeout,int opts) noex

	Arguments:
	fname		path to UNIX� domain socket to dial to
	timeout		timeout ('>=0' mean use one, '-1' means don't)
	opts		any dial opts

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/poll.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	POLLMULT
#define	POLLMULT	1000
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


/* exported subroutines */

int dialpass(cchar *fname,int timeout,int) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		rs = SR_NOSYS ;
		if constexpr (f_streams) {
	            cint	of = (O_WRONLY | O_NDELAY) ;
	            if ((rs = u_open(fname,of,0666)) >= 0) {
	                USTAT	sb ;
	                cint	fd_pass = rs ;
	                if ((rs = u_fstat(fd_pass,&sb)) >= 0) {
			    rs = SR_INVALID ;
	                    if (S_ISFIFO(sb.st_mode) || S_ISCHR(sb.st_mode)) {
			        rs = SR_OK ;
	                        if (timeout >= 0) {
	                            rs = waitready(fd_pass,timeout) ;
	                        } /* end if (timeout) */
	                        if (rs >= 0) {
	                            int		pipes[2] ;
	                            if ((rs = u_pipe(pipes)) >= 0) {
					cint	cmd = I_SENDFD ;
	                                fd = pipes[0] ;
	                                rs = u_ioctl(fd_pass,cmd,pipes[1]) ;
	                                u_close(pipes[1]) ;
	                                if (rs < 0) {
	                                    u_close(fd) ;
					    fd = -1 ;
	                                }
	                            } /* end if (u_pipe) */
	                        } /* end if (ok) */
			    } /* end if (type of file) */
	                } /* end if (stat) */
	                u_close(fd_pass) ;
	            } /* end if (u_open) */
		} /* end if-constexpr (f_streams) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (dialpass) */


/* local subroutines */

static int waitready(int fd,int timeout) noex {
	time_t		ti_timeout ;
	time_t		daytime = time(nullptr) ;
	int		rs = SR_OK ;
	int		f = FALSE ;
	if (timeout >= 0) {
	    POLLFD	polls[2] = {} ;
	    polls[0].fd = fd ;
	    polls[0].events = (POLLOUT | POLLWRBAND) ;
	    polls[1].fd = -1 ;
	    polls[1].events = 0 ;
	    ti_timeout = daytime + timeout ;
	    while (rs >= 0) {
	        cint	pollto = MIN(timeout,5) * POLLMULT ;
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
	            daytime = time(nullptr) ;
		    if (daytime >= ti_timeout) rs = SR_TIMEDOUT ;
	        }
	        if ((rs >= 0) && f) break ;
	    } /* end while */
	} /* end if (ok) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (waitready) */


