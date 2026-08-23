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

	= 2025-11-20, David A­D­ Morano
	I removed the old and unneeded (practically silly) code
	option of forking the |tty(1)| program to get a TTY-name.
	If I have to fork a program in order to get a TTY-name in
	this dat-and-age (it is 2025 now), then f*ck that!  The jig
	would be up already!

*/

/* Copyright © 1998,2011,2025 David A­D­ Morano.  All rights reserved. */

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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucdesc.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"termdevice.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#undef	MINBUFLEN
#define	MINBUFLEN	32

#ifndef	VARTERMDEV
#define	VARTERMDEV	"TERMDEV"
#endif


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
    } ; /* end struct (subinfo) */
} /* end namespace */

typedef int (subinfo::*subinfo_m)() noex ;


/* forward references */


/* local variables */

constexpr static subinfo_m	getnames[] = {
	&subinfo::var,
	&subinfo::ttyname,
} ; /* end array (subinfo_m) */

cbool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int termdevice(char *dbuf,int dlen,int fd) noex {
	int		rs = SR_NOENT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (dlen >= MINBUFLEN) ylikely {
		rs = SR_BADF ;
		if (fd >= 0) ylikely {
		    rs = SR_NOTTY ;
	            if (isatty(fd)) ylikely {
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
} /* end subroutine (termdevice) */


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
} /* end subroutine (subinfo_finish) */

int subinfo::finish() noex {
	return SR_OK ;
} /* end subroutine (subinfo_finish) */

int subinfo::var() noex {
	cchar		*vn = VARTERMDEV ;
	int		rs ;
	int		len = 0 ; /* return-value */
	if (ustat st1 ; (rs = u_fstat(fd,&st1)) >= 0) ylikely {
	    static cpcchar	valp = getenv(vn) ;
	    if (valp) {
		if (ustat st2 ; (rs = u_stat(valp,&st2)) >= 0) ylikely {
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
} /* end subroutine (getname_var) */

int subinfo::ttyname() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = uc_ttyname(fd,dbuf,dlen)) >= 0) ylikely {
	    len = lenstr(dbuf) ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getname_ttyname) */


