/* usys_ugetnisdom SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2025-06-17, David D-A- Morano
	As far as I know, Oracle Solaris® (formerly Sun-Microsystems
	Solaris®) does have the |getdomainame(3c)| subroutine.
	But it is hidden within the NSL library without being
	documented.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ugetnisdom

	Description:
	This provides a means to get the NIS domain-name from the
	operating system in an OS-independent way by regular callers.

	Synosis:
	int ugetnisdom(char *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of result
	<0		error code (- errno)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |getdomainname(3c)| */
#include	<cerrno>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usysflag.h>

#include	"usys_ugetnisdom.h"

namespace {
    sysret_t local_ugetnisdom(char *rbuf,int rlen) noex {
	csize		rsz = size_t(rlen) ;
	int		rs ;
	if ((rs = getdomainname(rbuf,rsz)) >= 0) {
	    rbuf[rsz] = '\0' ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
    } /* end subroutine */
}

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

/****
Solaris® has the subroutine, but w/ a different function signature.
The size of the supplied buffer is gieven as an |int| rather than
|size_z|.  This function signature (the one provided by Solaris®)
is actually the one we like the most, but we work with what we 
have.
****/

extern "C" {
    extern unixret_t getdomainname(char *,int) noex ;
}

sysret_t ugetnisdom(char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = getdomainname(rbuf,rlen)) >= 0) {
	    rbuf[rlen] = '\0' ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}

#elif	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

sysret_t ugetnisdom(char *rbuf,int rlen) noex {
	return local_ugetnisdom(rbuf,rlen) ;
}

#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)

sysret_t linux_ugetnisdom(char *rbuf,int rlen) noex {
	return local_ugetnisdom(rbuf,rlen) ;

#else

sysret_t ugetnisdom(char *rbuf,int rlen) noex {
	errno_t		ec = EFAULT ;
	if (rbuf) {
	    ec = EINVAL ;
	    if (rlen >= 0) {
		ec = ENOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return (- ec) ;
}

#endif /* which operating system */


