/* usys_ttynamerp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2026-10-15, David A-D- Morano
	I got rid of the usage of the subroutine |strlcpy(3c)|.
	That subroutine just did not fit our philosophy of
	good programming practices.  Its function signature was
	bad, and its semantics were bad.

*/

/* Copyright © 2001,2026 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	ttyname_rp

	Description:
	These subroutines support the tread-safe version of
	|ttyname(3c)| for those operating systems that do not have
	that.  Yes, those operating systems are brain-damaged in
	this day-and-age of ubiquous multi-threading (like ones
	with the initials "Apple-Darwin").

	Synopsis:
	errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex

	Arguments:
	fd		file-desciptor
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	==0		OK
	*errno*		UNIX® ERRNO code

	Notes: (obsoleted 2026-10-15, see revision note above)
	I am using the new (rumored to be coming as a standard)
	subroutine |strlcpy(3c)|.  I wrote my own version of this
	until it gets into the standard libraries.  It might be in
	someone's standard library already, but I do not have that
	in any of the operating systems I am working with.  And no,
	I do not like the function signature of that new interface
	(API).  I do not like it, but if it is going to become a
	new standard, it might get optimized for speed (like
	writting in hand-coded assembly language).  So that is
	pretty much the only reason I would use that interface
	(which I do not like).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® |ttyname_r(3c)| */
#include	<cerrno>		/* CSTD */
#include	<cstring>		/* CSTD |strncpy(3c)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<aflag.hh>		/* LIBU mutex-flag */

#include	"usys_darwin.h"
#include	"usys_strbcpy.hh"	/* |strbcpy(3usys)| */
#include	"usys_ttynamerp.h"

/* TTYNAMER start */
#if	defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0)

#if defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex {
	errno_t		ec = EBADF ; /* return-value */
	if (fd >= 0) ylikely {
	    ex = EFAULT ;
	    if (rbuf) ylikely {
		ec = EINVAL ;
		if (rlen > 0) ylikely {
		    csize rsize = size_t(rlen) ;
	            if (ttyname_r(fd,rbuf,rsize)) ylikely {
	                ec = errno ;
	            } /* end if */
		} /* end if */
	    } /* end if */
	} /* end if */
	return ec ;
} /* end subroutine */

#else /* all other operatring systems at this time */

errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex {
	errno_t		ec = EBADF ;
	if (fd >= 0) ylikely {
	    ec = EFAULT ;
	    if (rbuf) ylikely {
		ec = EINVAL ;
		if (rlen > 0) ylikely {
		    csize rsize = size_t(rlen) ;
	            ec = ttyname_r(fd,rbuf,rsize) ;
		} /* end if */
	    } /* end if */
	} /* end if */
	return ec ;
} /* end subroutine */

#endif /* which OS */

#else /* defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0) */

errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex {
    	using usys:sncpy ;
	errno_t		ec = EOK ; /* return-value */
	if (fd >= 0) ylikely {
	    if (rbuf) ylikely {
		if (rlen > 0) ylikely {
		    if (cchar *p = ttyname(fd)) ylikely {
			if (cint rs = sncpy(rbuf,rlen,p) ; rs < 0) ylikely {
			    ec = (neg rs) ;
			    rbuf[0] = eos ;
			} /* end */
		    } else {
			ec = errno ;
		    } /* end if (ttyname) */
		} else {
	            ec = EINVAL ;
		} /* end */
	    } else {
	        ec = EFAULT ;
	    } /* end */
	} else {
	    ec = EBADF ;
	} /* end */
	return ec ;
} /* end subroutine */

#endif /* defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0) */
/* TTYNAMER end */


