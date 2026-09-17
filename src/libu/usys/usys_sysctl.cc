/* usys_sysctl SUPPORT */
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
	int sysctl(int fd,void *rbuf,int rlen) noex

	Arguments:
	fd		file-descrptor for the open directory
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of result
	<0		error code (neg errno)

	Notes:
	1. Apple-Darwin has depracated their version of |msxmsglen(2)|.
	So we do not even pretend to try to support any version of
	|msxmsglen(2)| on Apple-Darwin.  I did try to use the
	Apple-Darwin version at the beginning, but that eventually
	preved to be unwise and futile.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdint>		/* CSTD |uint32_t| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */

#include	"usys_sysctl.h"

#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
/*----------------------------------------------------------------------------*/


/* Apple-Darwin aleady has this subroutine. */


/*----------------------------------------------------------------------------*/
#else /* all other OSes */
/*----------------------------------------------------------------------------*/


typedef uint32_t	ui ;
typedef size_t		sz ;
typedef void		vo ;

local errno_t bufck(void *bp,size_t *lp) noex {
    	errno_t		ec = 0 ;
	if (bp) {
	    if (lp) {
		ec = 0 ;
	    } else {
		ec = EFAULT ;
	    }
	}
	return ec ;
} /* end subroutine (bufck) */

local errno_t argck(void *obp,size_t *olp,void *nbp,size_t nbl) noex {
    	errno_t		ec = 0 ; /* return-value */
	if ((ec = bufck(obp,olp)) == 0) {
	    if (nbp) {
		if (nbl <= 0) ec = EOVERFLOW ;
	    }
	}
	return ec ;
} /* end subroutine (argck) */

unixret_t sysctl(int *nap,ui nal,void *obp,sz *olp,void *nbp,sz nbl) noex {
    	int		rc = -1 ;
	errno_t		ec = EFAULT ;
	if (nap) {
	    ec = EINVAL ;
	    if (nal > 0) {
		if ((ec = argck(obp,olp,nbp,nbl)) == 0) {
		    ec = ENOSYS ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (sysctl) */

unixret_t sysctlbyname(int *nap,ui nal,vo *obp,sz *olp,vo *nbp,sz nbl) noex {
    	int		rc = -1 ;
	errno_t		ec = EFAULT ;
	if (nap) {
	    ec = EINVAL ;
	    if (nal > 0) {
		if ((ec = argck(obp,olp,nbp,nbl)) == 0) {
		    ec = ENOSYS ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (sysctlbyname) */


/*----------------------------------------------------------------------------*/
#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */


