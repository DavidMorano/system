/* timewatch SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* watch time and variables simultaneously */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	timwatch

	Description:
	This structure provides a facility to watch variables for changes.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usupport.h>		/* |msleep(3u)| */
#include	<localmisc.h>

#include	"timewatch.hh"


