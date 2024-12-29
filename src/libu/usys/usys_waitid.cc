/* usys_waitid SUPPORT */
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

	For those operating systems that do not have |waitid(2)|
	we try to define it.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/wait.h>

/* WAITID start */
#if	(!defined(SYSHAS_WAITID)) || (SYSHAS_WAITID == 0)

#include	<climits>		/* |PATH_MAX| + |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |realpath(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysflag.h>
#include	<utypedefs.h>

#include	"usys_waitid.h"

unixret_t waitid(idtype_t,id_t,siginfo_t *,int) noex {
	errno = ENOSYS ;
	return -1 ;
}

#endif /* (!defined(SYSHAS_WAITID)) || (SYSHAS_WAITID == 0) */
/* WAITID end */


