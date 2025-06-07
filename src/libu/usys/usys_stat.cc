/* usys_stat SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Note:
	This needs to be emulated on Apple Darwin (since that OS
	does not have this call).  For whatever reason, Apple Darwin
	is way behind all others in getting new system or library
	calls.  This (pipe2(2)|) is just another in a line string
	of missing kernel or library calls.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* STAT start */
#if	(!defined(SYSHAS_STAT)) || (SYSHAS_STAT == 0)

#include	<cerrno>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysflag.h>
#include	<usysrets.h>

#include	"usys_stat.h"


/* nothing needed at the moment */


#endif /* (!defined(SYSHAS_STAT)) || (SYSHAS_STAT == 0) */
/* STAT end */


