/* usys_ufcntl SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	We define some missing operating system utilty functions.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<ctime>			/* |nanosleep(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |uintptr_t| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>		/* |ustime| */
#include	<utypealiases.h>	/* |TIMESPEC| */
#include	<usysflag.h>
#include	<timespec.h>		/* |timespec_load(3u)| */
#include	<localmisc.h>		/* |LEQUIV| */

#include	"usys_ucloseonexec.h"

#define	FCNTL_TO	30		/* general timeout */

constexpr ustime	onethousand = 1000 ;

namespace usys {
    sysret_t ufcntl(int fd,int cmd,uintptr_t anyarg) noex {
	TIMESPEC	ts{} ;
	int		to = FCNTL_TO ;
	int		rs ;
	if ((rs = timespec_load(&ts,onethousand,0)) >= 0) {
	    bool	fexit = false ;
	    repeat {
	        if ((rs = fcntl(fd,cmd,anyarg)) < 0) {
		    const errno_t	ec = errno ;
		    switch (ec) {
		    case EMFILE:
		    case ENFILE:
		    case ENOSPC:
		    case ENOBUFS:
		        if (to-- > 0) {
		            nanosleep(&ts,nullptr) ;
			} else {
			    fexit = true ;
			}
			break ;
		    case EAGAIN:
		    case EINTR:
		        break ;
		    default:
			fexit = true ;
			break ;
		    } /* end switch */
	            rs = (- ec) ;
	        } /* end if (error) */
	    } until ((rs >= 0) || fexit) ;
	} /* end if (timespec_load) */
	return rs ;
    } /* end if (ufcntl) */
    sysret_t ucloseonexec(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = ufcntl(fd,F_GETFD,0)) >= 0) {
	    int		fdflags = rs ;
	    f_previous = !!(fdflags & FD_CLOEXEC) ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            fdflags |= FD_CLOEXEC ;
	        } else {
	            fdflags &= (~ FD_CLOEXEC) ;
		}
	        rs = ufcntl(fd,F_SETFD,fdflags) ;
	    } /* end if (needed a change) */
	} /* end if */
	return (rs >= 0) ? f_previous : rs ;
    } /* end subroutine (ucloseonexec) */
    sysret_t unonblock(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = ufcntl(fd,F_GETFL,0)) >= 0) {
	    int		flflags = rs ;
	    f_previous = !!(flflags & O_NONBLOCK) ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            flflags |= O_NONBLOCK ;
	        } else {
	            flflags &= (~ O_NONBLOCK) ;
		}
	        rs = ufcntl(fd,F_SETFL,flflags) ;
	    } /* end if (needed a change) */
	} /* end if (u_fcntl) */
	return (rs >= 0) ? f_previous : rs ;
    } /* end subroutine (unonblock) */
}


