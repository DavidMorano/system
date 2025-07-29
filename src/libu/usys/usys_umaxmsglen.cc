/* usys_umaxmsglen SUPPORT */
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
	int umaxmsglen(int fd,void *rbuf,int rlen) noex

	Arguments:
	fd		file-descrptor for the open directory
	rbuf		result buffer pointer
	rlen		result buffer length

	Returns:
	>=0		length of result
	<0		error code (- errno)

	Notes:
	1. Apple-Darwin has depracated their version of |msxmsglen(2)|.
	So we do not even pretend to try to support any version of
	|msxmsglen(2)| on Apple-Darwin.  I did try to use the
	Apple-Darwin version at the beginning, but that eventually
	preved to be unwise and futile.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usysflag.h>

#include	"usys_umaxmsglen.h"

#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) 

#include	<sys/sysctl.h>

namespace libu {
    sysret_t umaxmsglen() noex {
	int		rs ;
	cchar		name[] = "kern.msgbuf" ;
	int32_t		val{} ;
	size_t		vsize = sizeof(int32_t) ;
	if ((rs = sysctlbyname(name,&val,&vsize,nullptr,0uz)) >= 0) {
	    rs = val ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
    } /* end subroutine (umaxmsglen) */
}

#else /* all other OSes */

namespace libu {
    sysret_t umaxmsglen() noex {
	return SR_NOSYS ;
    } /* end subroutine (umaxmsglen) */
}

#endif /* which OS */


