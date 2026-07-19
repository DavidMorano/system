/* retustime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* return UNIX-standard time */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"retustime.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

ustime retustime(void) noex {
    	custime	dt = time(nullptr) ;
	ustime	ust ;
	{
	    ust = ustime(dt) ;
	}
	return ust ;
} /* end subroutine (ustime) */


