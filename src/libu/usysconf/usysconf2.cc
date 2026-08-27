/* usysconf2 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	isysconf

	Description:
	This subroutine returns system-related values (as opposed
	to strings as |usysconfstr(3uc)| does).

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX |sysconf| stuff */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<atomic>		/* C++STD |atomic_int(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU */
#include	<usys.h>		/* LIBU |umaxmsglen(3u)| */
#include	<usysflag.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<errtimer.hh>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"usysconf.h"

module usysconf ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

extern "C++" {
    namespace libu {
        usysconfer	usysconf ;
    }
} /* end extern (C++) */


/* exported subroutines */


/* local subroutines */

namespace libu {
    void usysconfer_co::operator () (usysconfer *p,int m) noex {
	op = p ;
	w = m ;
    } ; /* end method (usysconfer_co::operator) */
    void usysconfer::dtor() noex {
    	if (finit) {
	    finit = false ;
	}
    } /* end method (usysconfer::dtor) */
    usysconfer_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case usysconfermem_tzname:
	        rs = usysconfval(sysconfcmd_maxtzname) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	if (rs >= 0) {
	    op->finit = true ;
	}
	return rs ;
    } /* end method (usysconfer_co::operator) */
} /* end namespace (libu) */


