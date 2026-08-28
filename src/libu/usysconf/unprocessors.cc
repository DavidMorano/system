/* unprocessors SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve the number of processprs on the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	unprocessors

	Description:
	Use the |sysconf(2)| favility to retrive the number of
	processors on the system.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usysconf.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

unprocer	unprocessors ;


/* exported subroutines */

int u_nprocessors(int w) noex {
    	int	rs = SR_INVALID ;
	int	rv = 0 ; /* return-value */
	if ((w >= 0) && (w < 2)) ylikely {
	    int cmd = -1 ;
	    switch (w) {
	    case 0:
    	        cmd = _SC_NPROCESSORS_ONLN ;
		break ;
	    case 1:
	        cmd = _SC_NPROCESSORS_CONF ;
		break ;
	    default:
		rs = SR_BUGCHECK ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && (cmd >= 0)) ylikely {
	       rs = u_sysconfval(cmd,nullptr) ;
	       rv = rs ;
	    } /* end if (ok) */
	} /* end if (valid) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (u_nprocessors) */


/* local subroutines */

unprocer::operator int () noex {
    	cint cmd = _SC_NPROCESSORS_ONLN ;
	return u_sysconfval(cmd,nullptr) ;
} /* end method (unprocer::operator) */

int unprocer::operator () (int w) noex {
    	return u_nprocessors(w) ;
} /* end method (unprocer::operator) */


