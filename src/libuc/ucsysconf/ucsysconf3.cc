/* ucsysconf3 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get system configuration information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_sysconfstr

	Description:
	This subroutine returns system-related values (as opposed
	to strings as |uc_sysconfstr(3uc)| does).

	Synopsis:
	int uc_sysconfstr(int req,long *rp) noex

	Arguments:
	req		requested value to return
	rp		pointer to value-type |long| for received value

	Returns:
	>=0		OK and this is the requested value
	<0		error (system-return)

	Name:
	uc_sysconfstr

	Description:
	This subroutine is NOT the same as the UNIX®-System version.
	This subroutine returns "OVERFLOW" when the user supplied
	buffer is not big enough to hold the associated value.

	Synopsis:
	int uc_sysconfstr(char *rbuf,int rlen,int req) noex

	Arguments:
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied value
	req		configuration value to request

	Returns:
	>0		valid and the value is returned with this length
	0		valid but there was no value associated
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>		/* |{xx}buflen| + |ustd_{x}(3u)| */
#include	<sysconfcmds.h>
#include	<localmisc.h>

module ucsysconf ;

import uconstants ;			/* |sysword| */

/* local defines */


/* imported namespaces */

using libu::sncpy ;			/* subroutine (template) */
using ustd::ustd_confstr ;		/* subroutine */
using ustd::ustd_confstr ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct cspath {
    	int	cs ;
	cchar	*path ;
	ccharp operator [] (int) noex ;
    } ; /* end struct (cspath) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr cspath	paths[] = {
	{ _CS_TMPDIR,		"/tmp" },
	{ _CS_PUBLICDIR,	"%h/var/public" },
	{ _CS_CACHEDIR,		"%h/var/cache" }
} ; /* end array (paths) */


/* exported variables */


/* exported subroutines */


/* local subroutines */

[[maybe_unused]] ccharp cspath::operator [] (int n) noex {
    	cchar	*rp = nullptr ;
	for (cauto &e : paths) {
	    if (n == e.cs) rp = e.path ;
	    if (rp) break ;
	} /* end for */
	return rp ;
} /* end method (cspath::operator) */

int ucsysconf::sysconfstr(int req) noex {
	return getstr(req) ;
} /* end method (ucsysconf::sysconfstr) */

int ucsysconf::getstr(int req) noex {
        int             rs ;
        if (req >= sysconfstr_synthetic) {
            rs = getstrsyn(req) ;
        } else {
            rs = getstd(req) ;
        }
        return rs ;
} /* end method (ucsysconf::sysstr) */

int ucsysconf::getstrsyn(int req) noex {
    	int		rs = SR_OK ;
	switch (req) {
	case _CS_TMPDIR:
	case _CS_PUBLICDIR:
	case _CS_CACHEDIR:
	    rs = sncpy(rbuf,rlen,sysword.w_tmpdir) ;
	    break ;
	} /* end switch */
	return rs ;
} /* end method (ucsysconf::sysstrsyn) */

int ucsysconf::mconfstr(int req) noex {
    	return ustd_confstr(req,rbuf,rlen) ;
} /* end subroutine (ucsysconf::mconfstr) */


