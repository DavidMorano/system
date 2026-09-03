/* modsys7 MODULE (implementation) */
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
	ms_nproc

	Description:
	Use the |sysconf(2)| favility to retrive the number of
	processors on the system.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU */

#include	"modsys.hh"

module modsys ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

msprocer	msnprocessors ;


/* exported subroutines */

int ms_nproc(int w) noex {
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
	       rs = ms_confval(cmd,nullptr) ;
	       rv = rs ;
	    } /* end if (ok) */
	} /* end if (valid) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (ms_nproc) */


/* local subroutines */

msprocer::operator int () noex {
    	cint cmd = _SC_NPROCESSORS_ONLN ;
	return ms_confval(cmd,nullptr) ;
} /* end method (msprocer::operator) */

int msprocer::operator () (int w) noex {
    	return ms_nproc(w) ;
} /* end method (msprocer::operator) */


