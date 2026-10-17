/* usys_ufstype SUPPORT */
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
	ufstype

	Description:
	This provides a means to retrieve the file-system type-string
	from the underlying operating system.

	Synosis:
	int ufstype(char *rbuf,int rlen,int fd) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	fd		file-descriptor

	Returns:
	>=0		length of result
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/mount.h>		/* |USTATFS| */
#include	<sys/statvfs.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_ufstype.h"

import libutil ;

/* imported namespaces */

using usys::sncpy ;			/* subroutine */
using usys::snwcpy ;			/* subroutine */

/* external subroutines */

extern "C" {
    extern int	u_fstatfs	(int,USTATFS *) noex ;
    extern int	u_fstatvfs	(int,USTATVFS *) noex ;
} /* end extern */

/* exported subroutines */

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

namespace libu {
    sysret_t ufstype(char *nbuf,int nlen,int fd) noex {
	int		rs = SR_FAULT ;
	if (nbuf) {
	    if (USTATVFS vsb ; (rs = u_fstatvfs(fd,&vsb)) >= 0) {
	        cchar	*cp = vsb.f_basetype ;
	        cint	cl = xstrnlen(vsb.f_basetype,FSTYPSZ) ;
	        rs = snwcpy(nbuf,nlen,cp,cl) ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ufstype) */
} /* end */

#elif	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

namespace libu {
    sysret_t ufstype(char *nbuf,int nlen,int fd) noex {
	int		rs = SR_FAULT ;
	if (nbuf) {
	    if (USTATFS sb ; (rs = u_fstatfs(fd,&sb)) >= 0) {
	        ccharp	cp = sb.f_fstypename ;
	        cint	cl = cstrnlen(sb.f_fstypename,MFSNAMELEN) ;
	        rs = snwcpy(nbuf,nlen,cp,cl) ;
	    } /* end */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ufstype) */
} /* end */

#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)

/****
Linux just does not have an easy way to get a file-system type c-string.
****/

namespace libu {
    sysret_t ufstype(char *rbuf,int rlen,int) noex {
	return sncpy(rbuf,rlen,"remote") ;
    } /* end */
} /* end namespace */

#else /* all other operating systems */

namespace libu {
    sysret_t ufstype(char *rbuf,int rlen,int) noex {
	return sncpy(rbuf,rlen,"remote") ;
    } /* end */
} /* end namespace */

#endif /* which operating system */


