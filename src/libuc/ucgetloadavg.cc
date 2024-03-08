/* ucgetloadavg SUPPORT */
/* lang=C++20 */

/* get load averages as floating doubles (relatively now standard!) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets the system load averages as double-floats!
	This is pretty much totally contrary to how they have ever
	been gotten before on any platform! Yes, the load averages
	are still just 32-bit integer quantities inside the kernel
	but the stupid new interface specified that the averages
	should be returned as double-floats. Yes, it is stupid but
	that is the way it works with these stupid interfaces!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucgetloadavg.h"
#include	"ugetloadavg.h"


/* local defines */


/* local namespaces */

using libuc::ucgetloadavg ;		/* internal library subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getloadavg(double *dla,int n) noex {
	return ucgetloadavg(dla,n) ;
}
/* end subroutine (uc_getloadavg) */


