/* usys_ugetdents SUPPORT */
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

	Name:
	ucloseonexec

	Description:
	This provides a means to get the NIS domain-name from the
	operating system in an OS-independent way by regualr callers.

	Synosis:
	int ugetdents(int fd,void *rbuf,int rlen) noex

	Arguments:
	fd		file-descrptor for the open directory
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of result
	<0		error code (neg errno)

	Notes:
	1. Apple-Darwin has depracated their version of |getdents(2)|.
	So we do not even pretend to try to support any version of
	|getdents(2)| on Apple-Darwin.  I did try to use the
	Apple-Darwin version at the beginning, but that eventually
	preved to be unwise and futile.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cnumeric>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */

#include	"usys_ugetdents.h"

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) 

namespace usys {
    sysret_t ugetdents(int fd,void *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    csize	vsz = size_t(rlen) ;
		    rs = SR_OK ;
		    if (unixret_t ru ; (ru = getdents(fd,rbuf,vsz)) >= 0) {
			len = saturate<int>(ru) ;
		    } else {
			rs = (neg errno) ;
		    } /* end if */
	        } /* end if (not-bad FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ugetdents) */
} /* end namespace */

#elif	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) 

namespace usys {
    sysret_t ugetdents(int fd,void *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    rs = SR_NOSYS ;
	        } /* end if (not-bad FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ugetdents) */
} /* end namespace */

#elif	defined(OSNAME_Linux) && (OSNAME_Liunx > 0) 

namespace usys {
    sysret_t ugetdents(int fd,void *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    csize	vsz = size_t(rlen) ;
		    rs = SR_OK ;
		    if (unixret_t ru ; (ru = getdents(fd,rbuf,vsz)) >= 0) {
			len = saturate<int>(ru) ;
		    } else {
			rs = (neg errno) ;
		    } /* end if (error) */
	        } /* end if (not-bad FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ugetdents) */
} /* end namespace */

#else /* any other operating system */

namespace usys {
    sysret_t ugetdents(int fd,void *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    rs = SR_NOSYS ;
	        } /* end if (not-bad FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ugetdents) */
} /* end namespace */

#endif /* which OS */


