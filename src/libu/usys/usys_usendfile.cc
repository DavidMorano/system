/* usys_usendfile SUPPORT */
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
	usendfile

	Description:
	This follows the version on both Linux and Solaris® (and
	its derivatives).

	Synosis:
	int usendfile(int fd,int s,off_t fo,size_t c) noex

	Arguments:
	fd		file-descriptor (to read from)
	s		socket (to write to)
	fo		starting file offset
	c		count of bytes to send

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
#include	<intsat.h>

#include	"usys_usendfile.h"

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) 

namespace usys {
    sysret_t usendfile(int fd,int s,off_t fo,size_t c) noex {
	int		rs = SR_BADFD ;
	int		len = 0 ;
	if ((fd >= 0) && (s >= 0) && (fo >= 0)) {
	    if (ssize_t ret ; (ret = sendfile(fd,s,fo,c)) >= 0) {
		rs = SR_OK ;
	        len = intsat(ret) ;
	    } else {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (usendfile) */
}

#elif	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) 

namespace usys {
    sysret_t usendfile(int fd,int s,off_t fo,size_t c) noex {
	int		rs = SR_BADFD ;
	int		len = 0 ;
	(void) c ;
	if ((fd >= 0) && (s >= 0) && (fo >= 0)) {
	    off_t	res = 0 ;
	    if ((rs = sendfile(fd,s,fo,&res,nullptr,0)) >= 0) {
	        len = intsat(res) ;
	    } else {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (usendfile) */
}

#elif	defined(OSNAME_Linux) && (SYSHAS_Liunx > 0) 

namespace usys {
    sysret_t usendfile(int fd,int s,off_t fo,size_t c) noex {
	int		rs = SR_BADFD ;
	int		len = 0 ;
	if ((fd >= 0) && (s >= 0) && (fo >= 0)) {
	    if (ssize_t ret ; (ret = sendfile(fd,s,fo,c)) >= 0) {
		rs = SR_OK ;
	        len = intsat(ret) ;
	    } else {
		rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (usendfile) */
}

#else /* any other operating system */

namespace usys {
    sysret_t usendfile(int fd,int s,off_t fo,size_t c) noex {
	int		rs = SR_BADFD ;
	int		len = 0 ;
	if ((fd >= 0) && (s >= 0) && (fo >= 0)) {
	    (void) c ;
	    rs = SR_NOSYS ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (usendfile) */
}

#endif /* which OS */


