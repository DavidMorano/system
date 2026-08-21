/* usys_streams SUPPORT */
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
#include	<sys/types.h>		/* POSIX */
#include	<climits>		/* CSTD |PATH_MAX| + |INT_MAX| */
#include	<cerrno>		/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |realpath(3c)| */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */

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
} /* end */

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
} /* end */

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
} /* end */

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
} /* end */

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
} /* end */

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
} /* end */

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
} /* end */

#endif /* (!defined(SYSHAS_STREAMS)) || (SYSHAS_STREAMS == 0) */
/* STREAMS end */


