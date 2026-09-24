/* usys_linux SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David A-D- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	I provide some missing system facilities in this module.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */

#include	"usys_linux.h"


#if	defined(OSNAME_Linux) && (OSNAME_Linux > 0)
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
#endif /* defined(OSNAME_Linux) && (OSNAME_Linux > 0) */
/* USYS_LINUX finish */


