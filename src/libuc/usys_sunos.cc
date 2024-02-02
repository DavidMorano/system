/* usys_sunos SUPPORT */
/* lang=C++20 */

/* define various sytem (global) variables */
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

#include	<envstandards.h>	/* ordered first to configure */

/* USYS_SUNOS start */
#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)

#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstring>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"usys_sunos.h"


/* local defines */


/* local typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


/* *NOTHING for now* */


#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
/* USYS_SUNOS finish */


