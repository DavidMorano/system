/* ufdlock_fcntl SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ufcntl

	Description:
	This provides special control for devices.

	Synopsis:
	int ufcntl(int fd,int cmd,...) noex

	Arguments:
	fd		file-descriptor
	cmd		command (request)
	...		any-argument

	Notes:
	1. Because of the subroutine signature of |u_fcntl(3u)|
	allowing for a variable number of arguments and also
	importantly allowing for arguments with any types, passing
	those arguments down to another subroutine is potentially
	problematic.  For little-ending machines, this is not a
	problem as it is such a superior machine byte ordering
	convention over the stupid and idiotic big-endian (idiotic)
	convention.  See the code below for how I handle this
	problem for big-endian machines.
	2. Note the intentional spelling of the structure below
	("ufuncter").

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |uintptr_t| */
#include	<cstdarg>
#include	<usystem.h>
#include	<usysflag.h>
#include	<endian.h>		/* used below (for something) */
#include	<localmisc.h>
#include	<ufiledesc.h>

#include	"ufdlock.h"


/* local defines */


/* imported namespaces */

using ufiledesc::ufiledescbase ;	/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ufuncter ;
    typedef int (ufuncter::*ufuncter_m)(int) noex ;
    struct ufuncter : ufiledescbase {
	ufuncter_m	m = nullptr ;
	uintptr_t	anyarg ;
	int		cmd ;
	ufuncter() noex { } ;
	ufuncter(int c,uintptr_t aa) noex : cmd(c), anyarg(aa) { } ;
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ;
	void submem(ufuncter_m mem) noex {
	    m = mem ;
	} ;
	int ifcntl(int) noex ;
    } ; /* end struct (ufuncter) */
} /* end namespace */


/* forward refeferences */

static sysret_t	try_fcntl(int,int,uintptr_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int ufcntl(int fd,int cmd,...) noex {
	va_list		ap ;
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    rs = SR_INVALID ;
	    if (cmd >= 0) {
	        va_begin(ap,cmd) ;
		if_constexpr (machendian.islittle) {
	            const uintptr_t	anyarg = va_arg(ap,uintptr_t) ;
		    rs = try_fcntl(fd,cmd,anyarg) ;
		} else if_constexpr (machendian.isbig) {
		    rs = SR_NOTSUP ;
		} else {
		    rs = SR_NOTSUP ;
		}
	        va_end(ap) ;
	    } /* end if (plausible-command) */
	} /* end if (good-FD) */
	return rs ;
    } /* end subroutine (ufcntl) */
} /* end namespace (libu) */


/* local subroutines */

static sysret_t try_fcntl(int fd,int cmd,uintptr_t anyarg) noex {
	ufuncter	fo(cmd,anyarg) ;
	fo.m = &ufuncter::ifcntl ;
	return fo(fd) ;
}
/* end subroutine (try_fcntl) */

int ufuncter::ifcntl(int fd) noex {
	int		rs ;
	if ((rs = fcntl(fd,cmd,anyarg)) == -1) {
	    rs = (- errno) ;
	}
	if (rs == SR_ACCESS) {
	    switch (cmd) {
	    case F_GETLK:
	    case F_SETLK:
	    case F_SETLKW:
	        rs = SR_AGAIN ;
		break ;
	    } /* end switch */
	} /* end if (old mess-up) */
	return rs ;
}
/* end method (ufuncter::ifcntl) */


