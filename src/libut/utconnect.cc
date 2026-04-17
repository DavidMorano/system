/* utconnect SUPPORT (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® XTI subroutine */
/* version %I% last-modified %G% */

#define	CF_ISCONN	0		/* change meaning of SR_ISCONN? */
#define	CF_BADSOLARIS	0		/* bad Solaris? */

/* revision history:

	= 1989-05-04, David A­D­ Morano
	I needed this subroutine (and the whole 'ut' set of
	subroutines) becauase one cannot connect to a 'ticotsord'
	transport with a UNIX® domain socket.  This may have been
	possible in the past but must have gone away sometime around
	1990 or so, maybe.  It probably went away when some smart
	person working on UNIX® System V Release 4 networking decided
	to make their sockets work more like the ones in BSD!

	= 1992-02-14, David A­D­ Morano
	I updated this subroutine with some stuff from the 'u_connect()'
	subroutine that has to do with a connect attempt after one
	was previously done but in which the previous one was done
	with the file descriptor in non-blocking mode and the latter
	one done with the file descriptor in blocking mode.  Does
	that ever really happen (anymore)?  I guess that it did at
	one time!  With all of the changes to the way sockets work
	now-a-days in different OSes, who can know what happens
	when (really)!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_connect

  	Description:
	This subroutine does what the socket counter part |u_connect()|
	does.  It is pretty much similar but not entirely the same (by
	any means).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>		/* |O_{x}| + |F_{x}| */
#include	<poll.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<localmisc.h>

#include	"utbase.hh"
#include	"uxti.h"


/* local defines */

#define	POLLTIMEOUT	(1 * 1000)	/* milliseconds */

#ifndef	CF_ISCONN
#define	CF_ISCONN	0		/* change meaning of SR_ISCONN? */
#endif
#ifndef	CF_BADSOLARIS
#define	CF_BADSOLARIS	0		/* bad Solaris? */
#endif


/* imported namespaces */

using libut::utbase ;


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	snxtierr(char *,int,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct connect_flags {
	uint	checkblock:1 ;
	uint	nonblock:1 ;
    } ;
    struct utbase_connect : utbase {
	UTCALL	*recvp ;
	utbase_connect(UTCALL *cp,UTCALL *rp) noex {
	    utcallp = cp ;
	    recvp = rp ;
	} ;
	int callstd(int) noex override final ;
    } ; /* end struct (utbase_connect) */
} /* end namespace */


/* forward references */

local int	checkconnect(int,connect_flags *) noex ;
local sysret_t	std_tconnect(int,UTCALL *,UTCALL *) noex ;


/* local variables */

cint		intmult = POLL_INTMULT ;

cbool		f_badsolaris	= CF_BADSOLARIS ;
cbool		f_isconn	= CF_ISCONN ;


/* exported variables */


/* exported subroutines */

int ut_connect(int fd,UTCALL *sndcall,UTCALL *rcvcall) noex {
    	utbase_connect uo(sndcall,rcvcall) ;
	return uo(fd) ;
} /* end subroutine (ut_connct) */


/* local subroutines */

#ifdef	COMMENT

int utbase_connect::callstd(int fd) noex {
	connect_flags	fl = { } ;
    	int		rs ;
	if ((rs = std_tconnect(fd,utcallp,recvp)) < 0) {
	    switch (rs) {
	   /* handle the case when it is still outstanding if we can */
	    case SR_ALREADY:
	    case SR_PROTO:
	        rs2 = checkconnect(fd,&f) ;
	        if (rs2 < 0) {
	            rs = rs2 ;
	            goto ret0 ;
	        }
	        if (! f.nonblock) {
	            POLLFD fds[2] = {} ;
	            int	nfds ;
	            int	looptimeout = (TO_CONNECT * intmult) ;
	            nfds = 0 ;
	            fds[nfds].fd = fd ;
	            fds[nfds].events = POLLOUT ;
	            fds[nfds].revents = 0 ;
	            nfds += 1 ;
	            while ((rs >= 0) && (looptimeout > 0)) {
	                if ((rs = u_poll(fds,nfds,POLLTIMEOUT)) >= 0) {
	                if (rs > 0) {
	                    cint	re = fds[0].revents ;
	                    rs = SR_OK ;
	                    if (re & POLLOUT) {
#if	CF_BADSOLARIS
	                        rs = lconnect(fd,sndcall,rcvcall) ;
				/* is it connected successfully yet? */
	                        if (rs == SR_ISCONN) {
	                            rs = SR_OK ;
				}
#endif /* CF_BADSOLARIS */
	                        if (rs >= 0) break ;
				/* is it still in progress? */
				if_constexpr (f_badsolaris) {
	                            if (rs == SR_ALREADY) {
	                                rs = SR_OK ;
				    }
				} /* end if_constexpr (f_badsolaris) */
	                    } else if (re & POLLHUP) {
	                        rs = SR_HANGUP ;
	                    } else if (re & POLLERR) {
	                        rs = SR_POLLERR ;
	                    } else if (re & POLLNVAL)
	                        rs = SR_NOTOPEN ;
	                    if (rs < 0) break ;
	                } /* end if */
	                looptimeout -= POLLTIMEOUT ;
	            } /* end while */
	            if (looptimeout <= 0) {
	                rs = SR_TIMEDOUT ;
		    }
	        } /* end if (waiting for connection to complete) */
	        break ;
	    case SR_ISCONN:
		if_constexpr (f_isconn) {
	            if ((rs = checkconnect(fd,&f)) < 0) {
	                if (! f.nonblock) {
	                    rs = SR_OK ;
		        } else {
			    fexit = true ;
		        }
		    } /* end if (checkconnect) */
		} /* end if_constexir (f_isconn) */
		break ;
	    } /* end switch */
	} /* end if (error) */

ret0:
	return rs ;

/* retry operation */
retry:
	msleep(1000) ;
	goto again ;
} /* end subroutine (itbase_connect::callstd) */

#else /* COMMENT */

int utbase_connect::callstd(int fd) noex {
    	if_constexpr (false) {
	    checkconnect(fd,nullptr) ;
	}
	return std_tconnect(fd,utcallp,recvp) ;
}

#endif /* COMMENT */


local int checkconnect(int fd,connect_flags *flp) noex {
	int		rs = SR_OK ;
	if (! flp->checkblock) {
	    flp->checkblock = true ;
	    if ((rs = u_fcntl(fd,F_GETFL,0)) >= 0) {
	        if (rs & O_NONBLOCK) {
	            flp->nonblock = true ;
	        }
	    }
	} /* end if (block check) */
	return rs ;
} /* end subroutine (checkconnect) */

local sysret_t std_tconnect(int fd,UTCALL *sndcall,UTCALL *rcvcall) noex {
	int		rs ;
	if ((rs = xti_connect(fd,sndcall,rcvcall)) < 0) {
	    switch (t_errno) {
	    case TACCES:
	        rs = SR_ACCES ;
	        break ;
	    case TBADADDR:
	        rs = SR_DESTADDRREQ ;
	        break ;
	    case TBADDATA:
	        rs = SR_TOOBIG ;
	        break ;
	    case TBADF:
	        rs = SR_NOTSOCK ;
	        break ;
	    case TBADOPT:
	        rs = SR_PROTOTYPE ;
	        break ;
	    case TBUFOVFLW:
	        rs = SR_OVERFLOW ;
	        break ;
	    case TLOOK:
	        rs = SR_LOOK ;
	        break ;
	    /* this is what is returned when connecting asynchronously */
	    case TNODATA:
	        rs = SR_OK ;
	        break ;
	    case TNOTSUPPORT:
	        rs = SR_OPNOTSUPP ;
	        break ;
	    case TOUTSTATE:
	        rs = SR_NOPROTOOPT ;
	        break ;
	    case TSYSERR:
	        rs = (- errno) ;
	        break ;
	    default:
		rs = SR_NOANODE ;
		break ;
	    } /* end switch */
	} /* end if */
	return rs ;
} /* end subroutine (std_tconnect) */


