/* usys_xxx */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We defines some system (global) variables in this module.


*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<errno.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"usys_xxx.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

/* USERATTR begin */
#if	(!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0)

#ifdef	__cplusplus
extern "C" {
#endif

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

#ifdef	__cplusplus
}
#endif

#endif /* (!defined(SYSHAS_USERATTR)) || (SYSHAS_USERATTR == 0) */
/* USERATTR end */

/* MEMPLOCK begin */
#if	(! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0)

#ifdef	__cplusplus
extern "C" {
#endif

int plock(int) noex {
	errno = ENOSYS ;
	return -1 ;
}

#ifdef	__cplusplus
}
#endif

#endif /* (! defined(SYSHAS_MEMPLOCK)) || (SYSHAS_MEMPLOCK == 0) */
/* MEMPLOCK end */


