/* usys_ucloseonexec SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We define some missing operating system utilty functions.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |uintptr_t| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysrets.h>
#include	<usysflag.h>
#include	<utypedefs.h>
#include	<localmisc.h>		/* |LEQUIV| */

#include	"usys_ucloseonexec.h"


static sysret_t ufcntl(int fd,int cmd,uintptr_t anyarg) noex {
	int		rs ;
	if ((rs = fcntl(fd,cmd,anyarg)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}

sysret_t ucloseonexec(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = ufcntl(fd,F_GETFD,0)) >= 0) {
	    int		fdflags = rs ;
	    f_previous = (fdflags & FD_CLOEXEC) ? 1 : 0 ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            fdflags |= FD_CLOEXEC ;
	        } else {
	            fdflags &= (~ FD_CLOEXEC) ;
		}
	        rs = ufcntl(fd,F_SETFD,fdflags) ;
	    } /* end if (needed a change) */
	} /* end if */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (ucloseonexec) */


