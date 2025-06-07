/* usys_xxx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David A-D- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_xxx

    	Description:
	We defines some system (global) variables in this module.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	"usys_xxx.h"

/*----------------------------------------------------------------------------*/
/* USERATTR begin */
#if	(!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0)

EXTERNC_begin

void setuserattr() noex { 
	errno = ENOSYS ;
}

void enduserattr() noex { 
	errno = ENOSYS ;
}

void free_userattr(userattr *ep) noex { 
	if (ep) {
	    errno = ENOSYS ;
	} else {
	    errno = EFAULT ;
	}
}

userattr *getuserattr() noex {
	errno = ENOSYS ;
	return nullptr ;
}

userattr *getusernam(const char *) noex  {
	errno = ENOSYS ;
	return nullptr ;
}

userattr *getuseruid(uid_t) noex {
	errno = ENOSYS ;
	return nullptr ;
}

EXTERNC_end

#endif /* (!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0) */
/* USERATTR end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* MEMPLOCK begin */
#if	(! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0)

EXTERNC_begin

int plock(int) noex {
	errno = ENOSYS ;
	return -1 ;
}

EXTERNC_end

#endif /* (! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0) */
/* MEMPLOCK end */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* LOADAVGINT begin */
#if	(! defined(SYSHAS_LOADAVGINT)) || (SYSHAS_LOADAVGINT == 0)
#ifdef	__cplusplus /* C++ only! */
namespace usys {
    unixret_t kloadavg(int *la,int n) noex {
    	int		rs = SR_FAULT ;
	if (la) {
	    rs = SR_INVALID ;
	    if ((n >= 0) && (n < 3)) {
    	        rs = SR_NOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
    	return rs ;
    }
} /* end namespec (usys) */
#endif /* __cplusplus (C++ only) */
#endif /* (! defined(SYSHAS_LOADAVGINT)) || (SYSHAS_LOADAVGINT == 0) */
/* LOADAVGINT end */
/*----------------------------------------------------------------------------*/


