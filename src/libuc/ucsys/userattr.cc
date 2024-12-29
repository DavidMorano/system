/* userattr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* USERATTR operating system data-base support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	userattr

	Description:
	This file contains the UNIX system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>

#include	"userattr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

/* USERATTR begin */
#if	defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0)


/* NOTHING */


#else /* defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0) */

extern void setuserattr() noex { 
	errno = ENOSYS ;
} ;

extern void enduserattr() noex { 
	errno = ENOSYS ;
} ;

extern void free_userattr(userattr *) noex { 
	errno = ENOSYS ;
}

extern userattr *getuserattr() noex {
	errno = ENOENT ;
	return nullptr ;
}

extern userattr *getusernam(cchar *) noex {
	errno = ENOENT ;
	return nullptr ;
}

extern userattr *getuseruid(uid_t) noex {
	errno = ENOENT ;
	return nullptr ;
}

#endif /* defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0) */
/* USERATTR end */


