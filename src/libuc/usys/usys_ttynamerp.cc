/* usys_ttynamerp SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |ttyname_r(3c)| */
#include	<cerrno>
#include	<cstring>		/* |strncpy(3c)| + |strlcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usysflag.h>
#include	<utypedefs.h>
#include	<usysrets.h>
#include	<clanguage.h>
#include	<aflag.hh>		/* mutex-flag */

#include	"usys_ttynamerp.h"

/* TTYNAMER start */
#if	defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0)

#if defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

int ttyname_rp(int fd,char *rbuf,int rlen) noex {
	csize		sz = size_t(rlen) ;
	int		ec = 0 ;
	if (char *p ; (p = ttyname_r(fd,rbuf,sz)) == nullptr) {
	    ec = errno ;
	}
	return ec ;
}

#else /* all other operatring systems at this time */

int ttyname_rp(int fd,char *rbuf,int rlen) noex {
	csize		sz = size_t(rlen) ;
	return ttyname_r(fd,rbuf,sz) ;
}

#endif /* which OS */

#else /* defined(SYSHAS_TTYNAMER) && (SYSHAS_TTYNAMER > 0) */

static aflag	mx ;

int ttyname_rp(int fd,char *rbuf,int rlen) noex {
	int		ec = 0 ;
	if (fd >= 0) {
	    if (rbuf) {
		if (rlen >= 0) {
		    csize	sz = size_t(rlen) ;
		    int		rc ;
		    if ((rc = mx.iguardbegin()) >= 0) {
		        if (char *p ; (p = ttyname(fd)) != nullptr) {
			    char	*dp = stpncpy(rbuf,p,sz) ;
			    *dp = '\0' ;
		        } else {
			    ec = errno ;
		        } /* end if (ttyname) */
			mx.iguardend(0 ;
		    } else {
		        ec = (- rc) ;
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


