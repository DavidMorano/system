/* usys_streams SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	General:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.  This particular file provides
	link-time symbols for emulating a broken UNIZ® STREAMS®
	interface.

	Name:
	getmsg
	getpmsg
	putmsg
	putpmsg

	Description:
	This module manages UNIX® STREAMS®.  These are the STREAMS®
	invented by Dennis Ritchie (of AT&T Bell Laboratories fame).
	The STREAMS® framework first appeared in UNIX® System V
	Release 3 (and in my opinion was a huge success there; I
	used it myself back in the day).  The STREAMS® framework
	was ported to UNIX® System V Release 4 by others.  For
	completeness, the people who ported STREAMS® to UNIX® System
	V Release 4 were: Robert Israel, Gil McGrath, Dave Olander,
	Her-Daw Che, and Maury Bach.

	Synopsis:
	unixret_t isasteam(int fd) noex
	unixret_t fattch(int fd,cchar *fname) noex
	unixret_t fdetch(cchar *fname) noex
	unixret_t getmsg(int fd,SB *cmp,SB *dmp,int *fp) noex
	unixret_t getpmsg(int fd,SB *cmp,SB *dmp,int *bp,int *fp) noex
	unixret_t putmsg(int fd,SB *cmp,SB *dmp,int flags) noex
	unixret_t putpmsg(int fd,SB *cmp,SB *dmp,int band,int flags) noex

	Arguments:
	fd		file-descriptor
	cmp		control-message-pointer
	dmp		data-message-pointer
	fp		pointer to flags (an integer)
	bp		pointer to priority-band (an integer)
	flag		flags
	fname		file-name of file to attach to or detach from

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<climits>		/* |PATH_MAX| + |INT_MAX| */
#include	<cerrno>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysflag.h>
#include	<utypedefs.h>
#include	<utypealiases.h>

#include	"usys_streams.h"

#define	SB	STRBUF

/* STREAMS start */
#if	(!defined(SYSHAS_STREAMS)) || (SYSHAS_STREAMS == 0)

unixret_t isastream(int fd) noex {
	unixret_t	rc = -1 ;
	errno_t		ec = 0 ;
	if (fd >= 0) {
	    ec = ENOSYS ;
	} else {
	    ec = EBADF ;
	}
	if (ec) {
	    errno = ec ;
	    rc = -1 ;
	}
	return rc ;
}

unixret_t fattach(int fd,cchar *fname) noex {
	unixret_t	rc = -1 ;
	errno_t		ec = 0 ;
	if (fd >= 0) {
	    if (fname) {
		if (fname[0]) {
		    ec = ENOSYS ;
		} else {
		    ec = EINVAL ;
		}
	    } else {
		ec = EFAULT ;
	    }
	} else {
	    ec = EBADF ;
	}
	if (ec) {
	    errno = ec ;
	    rc = -1 ;
	}
	return rc ;
}

unixret_t fdetach(cchar *fname) noex {
	unixret_t	rc = -1 ;
	errno_t		ec = 0 ;
	if (fname) {
	    if (fname[0]) {
		ec = ENOSYS ;
	    } else {
		ec = EINVAL ;
	    }
	} else {
	    ec = EFAULT ;
	}
	if (ec) {
	    errno = ec ;
	    rc = -1 ;
	}
	return rc ;
}

unixret_t getmsg(int fd,SB *cmp,SB *dmp,int *fp) noex {
	unixret_t	rc = -1 ;
	errno_t		ec = 0 ;
	if (fd >= 0) {
	    if (cmp && dmp && fp) {
		ec = ENOSYS ;
	    } else {
		ec = EFAULT ;
	    }
	} else {
	    ec = EBADF ;
	}
	if (ec) {
	    errno = ec ;
	    rc = -1 ;
	}
	return rc ;
}

unixret_t getpmsg(int fd,SB *cmp,SB *dmp,int *bp,int *fp) noex {
	unixret_t	rc = -1 ;
	errno_t		ec = 0 ;
	if (fd >= 0) {
	    if (cmp && dmp && bp && fp) {
		ec = ENOSYS ;
	    } else {
		ec = EFAULT ;
	    }
	} else {
	    ec = EBADF ;
	}
	if (ec) {
	    errno = ec ;
	    rc = -1 ;
	}
	return rc ;
}

unixret_t putmsg(int fd,SB *cmp,SB *dmp,int fl) noex {
	unixret_t	rc = -1 ;
	errno_t		ec = 0 ;
	(void) fl ;
	if (fd >= 0) {
	    if (cmp && dmp) {
		ec = ENOSYS ;
	    } else {
		ec = EFAULT ;
	    }
	} else {
	    ec = EBADF ;
	}
	if (ec) {
	    errno = ec ;
	    rc = -1 ;
	}
	return rc ;
}

unixret_t putpmsg(int fd,SB *cmp,SB *dmp,int bd,int fl) noex {
	unixret_t	rc = -1 ;
	errno_t		ec = 0 ;
	(void) fl ;
	(void) bd ;
	if (fd >= 0) {
	    if (cmp && dmp) {
		ec = ENOSYS ;
	    } else {
		ec = EFAULT ;
	    }
	} else {
	    ec = EBADF ;
	}
	if (ec) {
	    errno = ec ;
	    rc = -1 ;
	}
	return rc ;
}

#endif /* (!defined(SYSHAS_STREAMS)) || (SYSHAS_STREAMS == 0) */
/* STREAMS end */


