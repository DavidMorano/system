/* snflagsx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string version of the poll-event flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	snflags{x}

	Description:
	These subroutines create in the result buffer a list of
	symbolic representations of various "flag" values of various
	sorts.

	Synopsis:
	int snflagsfs	(char *dbuf,int dlen,ulong fl) noex
	int snflagsopen	(char *dbuf,int dlen,ulong fl) noex
	int snflagspoll	(char *dbuf,int dlen,ulong fl) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	fl		flags to conver to symbolic form

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/statvfs.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<snflags.h>
#include	<localmisc.h>

#include	"snflagsx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct flent {
	ulong		fl ;
	cchar		*s ;
} ;


/* forward references */

local int snflx(const flent *ftab,char *dbuf,int dlen,ulong fl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    if (snflags ss ; (rs = ss.start(dbuf,dlen)) >= 0) ylikely {
	        for (int i = 0 ; (rs >= 0) && ftab[i].s ; i += 1) {
	            if (fl & ftab[i].fl) {
	                rs = ss.addstr(ftab[i].s) ;
		    }
	        } /* end for */
	        rs1 = ss.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snflx) */


/* local variables */

constexpr flent		fl_fs[] = {
	{ ST_RDONLY, "RDONLY" },
	{ ST_NOSUID, "NOSUID" },
#ifdef	ST_NOTRUNC
	{ ST_NOTRUNC, "NOTRUNC" },
#endif
	{ 0, nullptr }
} ; /* end array (fl_fs) */

constexpr flent		fl_open[] = {
	{ O_APPEND, "APPEND" },
	{ O_CREAT, "CREAT" },
	{ O_EXCL, "EXCL" },
	{ O_TRUNC, "TRUNC" },
	{ O_NOCTTY, "NOCTTY" },
	{ O_SYNC, "SYNC" },
	{ O_DSYNC, "DSYNC" },
#ifdef	O_RSYNC
	{ O_RSYNC, "RSYNC" },
#endif
	{ O_NDELAY, "NDELAY" },
	{ O_NONBLOCK, "NONBLOCK" },
#ifdef	O_LARGEFILE
	{ O_LARGEFILE, "LARGE" },
#endif
#ifdef	O_DIRECTORY
	{ O_DIRECTORY, "DIRECTORY" },
#endif
#ifdef	O_DIRECT
	{ O_DIRECT, "DIRECT" },
#endif
#ifdef	O_TMPFILE
	{ O_TMPFILE, "TMPFILE" },
#endif
#ifdef	O_PRIV
	{ O_PRIV, "PRIV" },
#endif
#ifdef	O_CLOEXEC
	{ O_CLOEXEC, "CLOEXEC" },
#endif
#ifdef	O_MINMOD
	{ O_MINMOD, "MINMOD" },
#endif
#ifdef	O_MINFD
	{ O_MINFD, "MINFD" },
#endif
#ifdef	O_NETWORK
	{ O_NETWORK, "NETWORK" },
#endif
	{ 0, nullptr }
} ; /* end struct (fl_open) */

constexpr flent		fl_poll[] = {
	{ POLLIN, "IN" },
	{ POLLOUT, "OUT" },
	{ POLLERR, "ERR" },
	{ POLLHUP, "HUP" },
	{ POLLNVAL, "NVAL" },
#ifdef	POLLPRI
	{ POLLPRI, "PRI" },
#endif
#ifdef	POLLRDNORM
	{ POLLRDNORM, "RDNORM" },
#endif
#ifdef	POLLWRNORM
	{ POLLWRNORM, "WRNORM" },
#endif
#ifdef	POLLRDBAND
	{ POLLRDBAND, "RDBAND" },
#endif
#ifdef	POLLWRBAND
	{ POLLWRBAND, "WRBAND" },
#endif
#ifdef	POLLREMOVE
	{ POLLREMOVE, "REMOVE" },
#endif
	{ 0, nullptr }
} ; /* end array (fl_poll) */


/* exported variables */


/* exported subroutines */

int snflagsfs(char *dbuf,int dlen,ulong fl) noex {
    	return snflx(fl_fs,dbuf,dlen,fl) ;
}
/* end subroutine (snflagsfs) */

int snflagsopen(char *dbuf,int dlen,ulong fl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    if (snflags	ss ; (rs = ss.start(dbuf,dlen)) >= 0) ylikely {
	        cint	am = (fl & O_ACCMODE) ;
	        cchar	*ms = nullptr ;
	        switch (am) {
	        case O_RDONLY:
		    ms = "RDONLY" ;
		    break ;
	        case O_WRONLY:
		    ms = "WRONLY" ;
		    break ;
	        case O_RDWR:
		    ms = "RDWR" ;
		    break ;
	        default:
		    ms = "ACCINV" ; /* access invalid */
		    break ;
	        } /* end switch */
	        if (ms) {
		    rs = ss.addstr(ms) ;
	        }
	        for (int i = 0 ; (rs >= 0) && fl_open[i].s ; i += 1) {
	            if (fl & fl_open[i].fl) {
	                rs = ss.addstr(fl_open[i].s) ;
		    }
	        } /* end for */
	        rs1 = ss.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snflagsopen) */

int snflagspoll(char *dbuf,int dlen,ulong fl) noex {
    	return snflx(fl_poll,dbuf,dlen,fl) ;
}
/* end subroutine (snflagspoll) */


