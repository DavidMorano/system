/* u_poll SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */

#define	CF_INTR		0		/* do not return on an interrupt */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/select.h>
#include	<unistd.h>
#include	<poll.h>		/* <- might be fake if no |poll(2)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	UPOLL_NATIVE		(defined(SYSHAS_POLL) && (SYSHAS_POLL == 1))

#ifndef	UPOLL_RESOLUTION
#define	UPOLL_RESOLUTION	1000
#endif

#ifndef	INFTIM
#define	INFTIM		(-1)
#endif

#ifndef	POLLRDNORM
#define	POLLRDNORM	POLLIN
#endif

#ifndef	POLLRDBAND
#define	POLLRDBAND	0
#endif

#ifndef	POLLWRNORM
#define	POLLWRNORM	POLLOUT
#endif

#ifndef	POLLWRBAND
#define	POLLWRBAND	0
#endif

#define	TV		TIMEVAL

#define	FDS		fd_xset

#define	TO_AGAIN	10


/* external subroutines */

extern int	msleep(int) ;


/* local structures */

#if	defined(DARWIN)
typedef unsigned long		nfds_t ;
#endif


/* forward references */

#if	(UPOLL_NATIVE == 0)
static int	uselect(int,FDS *,FDS *,FDS *,TV *) ;
#endif


/* local variables */


/* exported subroutines */


#if	defined(SYSHAS_POLL) && (SYSHAS_POLL == 1)

int u_poll(POLLFD *fds,int n,int timeout) noex {
	const nfds_t	nfds = (nfds_t) n ; /* duh! 'int' wasn't good enough */
	int		rs ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = poll(fds,nfds,timeout)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
		    break ;
#if	CF_INTR
	        case SR_INTR:
	             break ;
#endif /* CF_INTR */
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (poll got an error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_poll) */

#else /* SYSHAS_POLL */

int u_poll(POLLFD *fds,int n,int timeout) noex {
	TV		tv ;
	FDS		rset, wset, eset ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		fd ;
	int		c = 0 ;

	if (n < 0)
	    return SR_INVALID ;

	if (n > FD_SETSIZE)
	    return SR_NOTSUP ;

	FD_ZERO(&rset) ;

	FD_ZERO(&wset) ;

	FD_ZERO(&eset) ;

	rs = SR_OK ;
	for (i = 0 ; i < n ; i += 1) {

	    fd = fds[i].fd ;
	    if (fd >= FD_SETSIZE) {
	        rs = SR_TOOBIG ;
	        break ;
	    }

	    fds[i].revents = 0 ;
	    if (fd >= 0) {

	        if ((fds[i].events & POLLIN) ||
	            (fds[i].events & POLLPRI) ||
	            (fds[i].events & POLLRDNORM) ||
	            (fds[i].events & POLLRDBAND))
	            FD_SET(fd,&rset) ;

	        if ((fds[i].events & POLLOUT) ||
	            (fds[i].events & POLLWRBAND))
	            FD_SET(fd,&wset) ;

	    }

	} /* end for */

	if (timeout != INFTIM) {
	    tv.tv_sec = timeout / UPOLL_RESOLUTION ;
	    tv.tv_usec = timeout % UPOLL_RESOLUTION ;
	} else {
	    tv.tv_sec = INT_MAX ;
	    tv.tv_usec = 0 ;
	}

	if (rs >= 0)
	    rs = uselect(n,&rset,&wset,&eset,&tv) ;

	if (rs >= 0) {
	    int	v ;
	    int	f ;

	    c = 0 ;
	    for (i = 0 ; i < n ; i += 1) {

	        fd = fds[i].fd ;
	        if (fd >= 0) {

	            f = FALSE ;
	            if (FD_ISSET(fd,&rset)) {
	                f = TRUE ;
	                if (fds[i].events & POLLIN)
	                    fds[i].revents |= POLLIN ;

#ifdef	COMMENT
	                if (fds[i].events & POLLPRI)
	                    fds[i].revents |= POLLPRI ;
#endif /* COMMENT */

	                if (fds[i].events & POLLRDNORM)
	                    fds[i].revents |= POLLRDNORM ;

	                if (fds[i].events & POLLRDBAND) {
	                    rs1 = u_ioctl(fd,SIOCATMARK,&v) ;
	                    if ((rs1 < 0) || (v > 0)) {
	                        fds[i].revents |= POLLRDBAND ;
			    }
	                }

	            } /* end if */

	            if (FD_ISSET(fd,&wset)) {
	                f = TRUE ;
	                if (fds[i].events & POLLOUT) {
	                    fds[i].revents |= POLLOUT ;
			}
	                if (fds[i].events & POLLWRBAND) {
	                    fds[i].revents |= POLLWRBAND ;
			}
	            }

	            if (FD_ISSET(fd,&eset)) {
	                f = TRUE ;
	                fds[i].revents |= POLLERR ;
	            }

	            if (f)
	                c += 1 ;

	        } /* end if */

	    } /* end for */

	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (u_poll) */

static int uselect(int nfds,FDS *rfds,FDS *wfds,FDS *efds,TV *tp) noex {
	int		rs ;
	if ((rs = select(nfds,rfds,wfds,efds,tp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uselect) */

#endif /* SYSHAS_POLL */


