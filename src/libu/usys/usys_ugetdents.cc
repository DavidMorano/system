/* usys_ugetdents SUPPORT */
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

	Name:
	ucloseonexec

	Description:
	This provides a means to get the NIS domain-name from the
	operating system in an OS-independent way by regualr callers.

	Synosis:
	int ugetdents(int fd,dirent_t *rbuf,int rlen) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of result
	<0		error code (- errno)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usysflag.h>

#include	"usys_ugetdents.h"

#if	defined(OSNAME_SunOS) && (SYSHAS_SunOS > 0) 

namespace usys {
    sysret_t ugetdents(int fd,dirent_t *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    csize	vsz = size_t(rlen) ;
		    rs = SR_OK ;
		    if (int rc ; (rc = getdents(fd,rbuf,vsz)) < 0) {
			rs = (- errno) ;
		    }
	        } /* end if (not-bad FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ugetdents) */
}

#elif	defined(OSNAME_Darwin) && (SYSHAS_Darwin > 0) 

namespace usys {
    sysret_t ugetdents(int fd,dirent_t *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    csize	vsz = size_t(rlen) ;
		    char	*vbuf = charp(rbuf) ;
		    off_t	off{} ;
		    rs = SR_OK ;
		    if (int rc ; (rc = getdirentries64(fd,vbuf,vsz,&off)) < 0) {
			rs = (- errno) ;
		    }
	        } /* end if (not-bad FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ugetdents) */
}

#elif	defined(OSNAME_Linux) && (SYSHAS_Liunx > 0) 

namespace usys {
    sysret_t ugetdents(int fd,dirent_t *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		rs = SR_BADF ;
		if (fd >= 0) {
		    csize	vsz = size_t(rlen) ;
		    rs = SR_OK ;
		    if (int rc ; (rc = getdents(fd,rbuf,vsz)) < 0) {
			rs = (- errno) ;
		    }
	        } /* end if (not-bad FD) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (ugetdents) */
}

#else /* any other operating system */

namespace usys {
    sysret_t ugetdents(int fd,dirent_t *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
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
}

#endif /* which OS */


