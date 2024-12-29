/* usys_ufstype SUPPORT */
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
#include	<sys/param.h>
#include	<sys/mount.h>		/* |struct statfs| */
#include	<sys/statvfs.h>
#include	<unistd.h>		/* |getdomainname(3c)| */
#include	<cerrno>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usysflag.h>
#include	<usupport.h>

#include	"usys_ufstype.h"


/* imported namespaces */

using libu::sncpy ;
using libu::snwcpy ;

/* external subroutines */

extern "C" {
    extern int	u_fstatfs(int,STATFS *) noex ;
    extern int	u_fstatvfs(int,STATVFS *) noex ;
}

/* exported subroutines */

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

namespace libu {
    sysret_t ufstype(char *nbuf,int nlen,int fd) noex {
	int		rs = SR_FAULT ;
	if (nbuf) {
	    STATVFS	vsb ;
	    if ((rs = u_fstatvfs(fd,&vsb)) >= 0) {
	        cchar	*cp = vsb.f_basetype ;
	        cint	cl = strnlen(vsb.f_basetype,FSTYPSZ) ;
	        rs = snwcpy(nbuf,nlen,cp,cl) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ufstype) */
}

#elif	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

namespace libu {
    sysret_t ufstype(char *nbuf,int nlen,int fd) noex {
	int		rs = SR_FAULT ;
	if (nbuf) {
	    STATFS	sb ;
	    if ((rs = u_fstatfs(fd,&sb)) >= 0) {
	        cchar	*cp = sb.f_fstypename ;
	        cint	cl = strnlen(sb.f_fstypename,MFSNAMELEN) ;
	        rs = snwcpy(nbuf,nlen,cp,cl) ;
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (ufstype) */
}

#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)

/****
Linux just does not have an easy way to get a file-system type c-string.
****/

namespace libu {
    sysret_t ufstype(char *rbuf,int rlen,int) noex {
	return sncpy(rbuf,rlen,"remote") ;
    }
}

#else /* all other operating systems */

namespace libu {
    sysret_t ufstype(char *rbuf,int rlen,int) noex {
	return sncpy(rbuf,rlen,"remote") ;
    }
}

#endif /* which operating system */


