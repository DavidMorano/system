/* usys_ttynamerp SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	ttyname_rp

	Description:
	These subroutines support the tread-safe version of
	|ttyname(3c)| for those operating systems that do not have
	that.  Yes, those operating systems are brain-damaged in
	this day-and-age of ubiquous multi-threading.

	Synopsis:
	errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex {

	Arguments:
	fd		file-desciptor
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	==0		OK
	*errno*		UNIX® ERRNO code

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |ttyname_r(3c)| */
#include	<cerrno>
#include	<cstring>		/* |strncpy(3c)| + |strlcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysflag.h>
#include	<usysrets.h>
#include	<aflag.hh>		/* mutex-flag */

#include	"usys_ttynamerp.h"

/* TTYNAMER start */
#if	defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0)

#if defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex {
	csize		sz = size_t(rlen) ;
	errno_t		ec = 0 ;
	if (char *p ; (p = ttyname_r(fd,rbuf,sz)) == nullptr) {
	    ec = errno ;
	}
	return ec ;
}

#else /* all other operatring systems at this time */

errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex {
	csize		sz = size_t(rlen) ;
	return ttyname_r(fd,rbuf,sz) ;
}

#endif /* which OS */

#else /* defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0) */

static constinit aflag	mx ;

errno_t ttyname_rp(int fd,char *rbuf,int rlen) noex {
	errno_t		ec = 0 ;
	if (fd >= 0) {
	    if (rbuf) {
		if (rlen >= 0) {
		    csize	sz = size_t(rlen) ;
		    int		rs ;
		    int		rs1 ;
		    if ((rs = mx.lockbegin) >= 0) {
		        if (char *p ; (p = ttyname(fd)) != nullptr) {
			    char	*dp = stpncpy(rbuf,p,sz) ;
			    *dp = '\0' ;
		        } else {
			    ec = errno ;
		        } /* end if (ttyname) */
			rs1 = mx.lockend ;
			if (rs >= 0) rs = rs1 ;
			if (rs < 0) {
			    ec = (- rs) ;
			}
		    } else {
		        ec = (- rs) ;
		    } /* end if (aflag-mx) */
		} else {
	            ec = EINVAL ;
		}
	    } else {
	        ec = EFAULT ;
	    }
	} else {
	    ec = EBADF ;
	}
	return ec ;
}

#endif /* defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0) */
/* TTYNAMER end */


