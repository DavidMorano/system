/* getnprocessors */
/* lang=C++20 */

/* get the number of currently online processors */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnprocessors

	Description:
	This subroutine gets and returns the current number of
	processors that are online.

	Synopsis:
	int getnprocessors(cchar **envv,int w) noex

	Arguments:
	- envv		pointer to process environment
	- w		which value:
				0=and-source
				1=local-environment
				2=global-envoronment
				3=system

	Returns:
	>=0		the number of processors that are currently online
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>		/* <- for |getenv(3c)| */
#include	<usystem.h>
#include	<getourenv.h>
#include	<cfdec.h>
#include	<isnot.h>
#include	<localmisc.h>


/* local defines */

#ifndef	VARNCPU
#define	VARNCPU		"NCPU"
#endif


/* local namespace */


/* local typedefs */


/* external subroutines */


/* external variables */

extern cchar	**environ ;


/* local structues */

enum whiches {
	w_all,
	w_locenv,
	w_gloenv,
	w_ucsys,
	w_def,
	w_overlast
} ;

namespace {
    struct procer ;
    typedef int (procer::*procer_m)() noex ;
    struct procer {
	cchar		**envv{} ;
	int		w = 0 ;
	procer(cchar **p,int aw) noex : envv(p), w(aw) { } ;
	int locenv() noex ;
	int gloenv() noex ;
	int ucsys() noex ;
	int def() noex ;
    } ; /* end struct (procer) */
}


/* forward references */


/* local variables */

static procer_m		procers[] = {
	&procer::locenv,
	&procer::gloenv,
	&procer::ucsys,
	&procer::def,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int getnprocessors(cchar **envv,int w) noex {
	int		rs = SR_INVALID ;
	if (w >= 0) {
	    procer	po(envv,w) ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs == 0) && procers[i] ; i += 1) {
	        procer_m	m = procers[i] ;
		rs = (po.*m)() ;
	    } /* end for */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (getnprocessors) */


/* local subroutines */

static int decout(cchar *valp) noex {
	int		rs ;
	int		v{} ;
	if ((rs = cfdeci(valp,-1,&v)) >= 0) {
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
	        cchar	*vn = VARNCPU ;
	        cchar	*cp ;
	        if ((cp = getourenv(envv,vn)) != nullptr) {
		    rs = decout(cp) ;
	        } /* end if (local-environment) */
	    } /* end if (selected) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (procer::locent) */

int procer::gloenv() noex {
	int		rs = SR_OK ;
	if ((w == w_all) || (w == w_gloenv)) {
	    cchar	*vn = VARNCPU ;
	    cchar	*cp ;
	    if ((cp = getenv(vn)) != nullptr) {
		rs = decout(cp) ;
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


