/* termdevice SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find the name of the device for the given file descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-15, David A­D­ Morano
	This code was originally written. This was also inspired
	by the fact that the Sun Solaris 2.5.1 POSIX version of
	|ttyname_r(3c)| does not appear to work.  I got the idea for
	this subroutine from the GNU standard C library implementation.
	It seems like Slowlaris 5.x certainly had a lot of buggy
	problems (sockets, I-O, virtual memory, more)!

	= 2011-10-12, David A­D­ Morano
	I am changing the order of attempts to put |ttyname_r(3c)|
	before forking a process. Even though we are still on
	Slowlaris we hope that |ttyname_r(3c)| is now working
	properly!

*/

/* Copyright © 1998,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	termdevice

	Description:
	Store at most BUFLEN character of the pathname, if the
	terminal FD is open, in the caller specified buffer.

	Synopsis:
	int termdevice(int fd,char *dbuf,int dben) noex

	Arguments:
	fd		file descriptor
	dbuf		buffer to store name
	dlen		length of buffer to store name

	Return:
	>0	length of device name
	0	on success
	<0	otherwise error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<isnot.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"termdevice.h"

import libutil ;

/* local defines */

#undef	MINBUFLEN
#define	MINBUFLEN	32

#ifndef	DEVDNAME
#define	DEVDNAME	"/dev"
#endif

#ifndef	TTYFNAME
#define	TTYFNAME	"/dev/tty"
#endif

#ifndef	NULLFNAME
#define	NULLFNAME	"/dev/null"
#endif

#ifndef	PROG_TTY
#define	PROG_TTY	"/usr/bin/tty"
#endif

#ifndef	VARTERMDEV
#define	VARTERMDEV	"TERMDEV"
#endif

#ifndef	VARAUDIODEV
#define	VARAUDIODEV	"AUDIODEV"
#endif

#define	TO_READ		20		/* timeout waiting for sub-process */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo_fl {
	uint		init:1 ;
    } ;
    struct subinfo {
	char		*dbuf ;
	subinfo_fl	fl ;
	int		dlen ;
	int		fd ;
	int start(char *,int,int) noex ;
	int finish() noex ;
	int var() noex ;
	int ttyname() noex ;
	int fork() noex ;
    } ; /* end struct (subinfo) */
} /* end namespace */

typedef int (subinfo::*subinfo_m)() noex ;


/* forward references */


/* local variables */

constexpr static subinfo_m	getnames[] = {
	&subinfo::var,
	&subinfo::ttyname,
	&subinfo::fork
} ; /* end array (subinfo_m) */

cbool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int termdevice(char *dbuf,int dlen,int fd) noex {
	int		rs = SR_NOENT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (dlen >= MINBUFLEN) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    rs = SR_NOTTY ;
	            if (isatty(fd)) {
	                dbuf[0] = '\0' ;
	                if (subinfo si ; (rs = si.start(dbuf,dlen,fd)) >= 0) {
	                    for (cauto &m : getnames) {
	                        rs = (si.*m)() ;
	                        if (rs != 0) break ;
	                    } /* end for */
	                    rs1 = si.finish() ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (subinfo) */
	            } /* end if (isatty) */
	        } /* end if (FD OK) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termdevice) */


/* local subroutines */

int subinfo::start(char *db,int dl,int afd) noex {
	int		rs = SR_FAULT ;
	if (db) ylikely {
	    rs = SR_OK ;
	    fd = afd ;
	    dbuf = db ;
	    dlen = dl ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

int subinfo::finish() noex {
	return SR_OK ;
}
/* end subroutine (subinfo_finish) */

int subinfo::var() noex {
	cchar		*vn = VARTERMDEV ;
	int		rs ;
	int		len = 0 ; /* return-value */
	if (ustat st1 ; (rs = u_fstat(fd,&st1)) >= 0) {
	    static cpcchar	valp = getenv(vn) ;
	    if (valp) {
		if (ustat st2 ; (rs = u_stat(valp,&st2)) >= 0) {
	            rs = SR_NOENT ;
	            if (st1.st_rdev == st2.st_rdev) {
	                rs = sncpy1(dbuf,dlen,valp) ;
	                len = rs ;
	            }
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (environment variable worked out!) */
	} /* end if (stat) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_var) */

int subinfo::ttyname() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = uc_ttyname(fd,dbuf,dlen)) >= 0) {
	    len = lenstr(dbuf) ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_ttyname) */

int subinfo::fork() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if_constexpr (f_comment) {
	    int		pfds[2] ;
	    if ((rs = u_pipe(pfds)) >= 0) {
	        cint	tlen = MAXPATHLEN ;
	        int	cs ;
	        char	tbuf[tlen + 2] = {} ;
	        if ((rs = uc_fork()) == 0) {
	            cchar	*av[3] ;
	            u_close(pfds[0]) ; /* not used */
	            for (int i = 0 ; i < 3 ; i += 1) {
		        u_close(i) ;
		    }
	            u_dup(fd) ;
	            u_close(fd) ;		/* done using it */
	            u_dup(pfds[1]) ;		/* standard output */
	            u_close(pfds[1]) ;		/* done using it */
	            u_open(NULLFNAME,O_WRONLY,0666) ;
	            av[0] = "tty" ;
	            av[1] = nullptr ;
	            u_execv(PROG_TTY,av) ;
	            uc_exit(EX_NOEXEC) ;
	        } else if (rs > 0) {
	            const pid_t	pid_child = rs ;
	            cint	to = TO_READ ;
	            cint	opts = FM_TIMED ;
	            u_close(pfds[1]) ;	/* not used */
	            pfds[1] = -1 ;
	            if ((rs = uc_reade(pfds[0],tbuf,tlen,to,opts)) > 0) {
	                len = rs ;
		        int i ; /* used-afterwards */
	                for (i = 0 ; i < len ; i += 1) {
	                    if (tbuf[i] == '\n') break ;
	                }
	                if (i == len) rs = SR_RANGE ;
	                tbuf[i] = '\0' ;
	                rs = snwcpy(dbuf,dlen,tbuf,i) ;
	                len = rs ;
		    } /* end uc_ucreade) */
	    	    rs1 = u_waitpid(pid_child,&cs,0) ;
		    if (rs >= 0) rs = rs1 ;
	        } else {
	            rs = SR_BADE ;
	        }
	        u_close(pfds[0]) ;
	        if (pfds[1] >= 0) {
	            u_close(pfds[1]) ;
	        }
	    } /* end if (pipe) */
	} /* end if_constexpr (f_comment) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getname_fork) */


