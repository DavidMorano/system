/* getnprocessors SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the number of currently online processors */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnprocessors

	Description:
	This subroutine gets and returns the current number of
	processors that are online.

	Synopsis:
	int getnprocessors(mainv envv,int w) noex

	Arguments:
	- envv		pointer to process environment
	- w		which value:
				0=and-source
				1=local-environment
				2=global-envoronment
				3=system

	Returns:
	>=0		the number of processors that are currently online
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<getourenv.h>
#include	<cfdec.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getnprocessors.h"

import uconstants ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structues */

enum whiches {
	w_all,
	w_locenv,
	w_gloenv,
	w_ucsys,
	w_def,
	w_overlast
} ; /* end enum (whiches) */

namespace {
    struct procer ;
    typedef int (procer::*procer_m)() noex ;
    struct procer {
	mainv		envv{} ;
	int		w = 0 ;
	procer(mainv p,int aw) noex : envv(p), w(aw) { } ;
	int locenv() noex ;
	int gloenv() noex ;
	int ucsys() noex ;
	int def() noex ;
    } ; /* end struct (procer) */
}


/* forward references */


/* local variables */

constexpr procer_m	procers[] = {
	&procer::locenv,
	&procer::gloenv,
	&procer::ucsys,
	&procer::def
} ; /* end array (procers) */


/* exported variables */


/* exported subroutines */

int getnprocessors(mainv envv,int w) noex {
	int		rs = SR_INVALID ;
	if (w >= 0) {
	    procer	po(envv,w) ;
	    rs = SR_OK ;
	    for (cauto &m : procers) {
		rs = (po.*m)() ;
		if (rs != 0) break ;
	    } /* end for */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (getnprocessors) */


/* local subroutines */

static int decout(cchar *valp) noex {
	int		rs ;
	if (int v{} ; (rs = cfdeci(valp,-1,&v)) >= 0) {
	    rs = v ;
	} else if (isNotValid(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (decout) */

int procer::locenv() noex {
	int		rs = SR_OK ;
	if (envv) {
	    if ((w == w_all) || (w == w_locenv)) {
	        cchar	*vn = varname.ncpu ;
	        if (cchar *cp ; (cp = getourenv(envv,vn)) != nullptr) {
		    rs = decout(cp) ;
	        } /* end if (local-environment) */
	    } /* end if (selected) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (procer::locent) */

int procer::gloenv() noex {
	cchar		*vn = varname.ncpu ;
	int		rs = SR_OK ;
	if ((w == w_all) || (w == w_gloenv)) {
	    static cchar	*valp = getenv(vn) ;
	    if (valp) {
		rs = decout(valp) ;
	    } /* end if (global-environment) */
	} /* end if (selected) */
	return rs ;
}
/* end method (procer::gloent) */

int procer::ucsys() noex {
	int		rs = SR_OK ;
	if ((w == w_all) || (w == w_ucsys)) {
	    rs = uc_nprocessors(0) ;
	} /* end if (selected) */
	return rs ;
}
/* end method (procer::ucsys) */

int procer::def() noex {
	int		rs = SR_OK ;
	if ((w == w_all) || (w == w_def)) {
	    rs = 1 ;
	} /* end if (selected) */
	return rs ;
}
/* end method (procer::def) */


