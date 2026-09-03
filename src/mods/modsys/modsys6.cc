/* modsys6 MODULE (module-implementation-unit) */
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
	modsys6

	Description:
	This subroutine returns system-related values (as opposed
	to strings as |modsysconfstr(3uc)| does).

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
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU */

#include	"modsys.hh"

module modsys ;

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

modsysconfer	msconf ;


/* exported subroutines */


/* local subroutines */

    void modsysconfer_co::operator () (modsysconfer *p,int m) noex {
	op = p ;
	w = m ;
    } ; /* end method (modsysconfer_co::operator) */

    void modsysconfer::dtor() noex {
    	if (finit) {
	    finit = false ;
	}
    } /* end method (modsysconfer::dtor) */

    modsysconfer_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    int sc = -1 ;
	    switch (w) {
	    case modsysconfermem_maxnodelen:
	        sc = _SC_NODENAME_MAX ;
		break ;
	    case modsysconfermem_maxnamelen:
	        sc = _SC_NAME_MAX ;
	        break ;
	    case modsysconfermem_maxpathlen:
	        sc = _SC_PATH_MAX ;
	        break ;
	    case modsysconfermem_maxlinelen:
	        sc = _SC_LINE_MAX ;
	        break ;
	    case modsysconfermem_tzname:
	        sc = _SC_TZNAME_MAX ;
	        break ;
	    } /* end switch */
	    if (sc >= 0) {
	        rs = ms_confval(sc) ;
	    }
	} /* end if (non-null) */
	if (rs >= 0) {
	    op->finit = true ;
	} /* end if (ok) */
	return rs ;
    } /* end method (modsysconfer_co::operator) */


