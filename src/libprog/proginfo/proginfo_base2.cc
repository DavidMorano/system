/* proginfo2 MODULE (Module-Implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

	= 2026-10-20, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	proginfo

	Description:
	This module provides a very basic sets of simple subroutines
	for basic program management.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strw.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<prognamevar.hh>	/* LIBDAM */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo.base ;

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct execnamer {
	proginfo	*pip ;
	ccharp     	efn ;
	operator int 	() noex ;
	int tryargz	() noex ;
	int tryshell	() noex ;
	int tryenvfn	() noex ;
	int trysys	() noex ;
	execnamer(proginfo *p) noex : pip(p), efn(nullptr) { } ;
    } ; /* end struct (execnamer) */
    typedef int (execnamer::*execnamer_m)() noex ;
} /* end namespace */


/* forward references */

local ccharp	getunderpath(ccharp) noex ;


/* local variables */

constexpr execnamer_m	mfuns[] = {
    	&execnamer::tryargz,
    	&execnamer::tryshell,
    	&execnamer::tryenvfn,
    	&execnamer::trysys
} ; /* end array */

statcexpr int		chx_star	= CH_STAR ;
statcexpr bool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int proginfo_progename(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip->progename.valp == nullptr) {
	    if (execnamer eo(pip) ; (rs = eo) >= 0) {
		pip->progename.valp = eo.efn ;
	    } /* end if (execnamer) */
	} /* end if (needed) */
	return rs ;
} /* end subroutine (proginfo_progename) */


/* local subroutines */

execnamer::operator int () noex {
    	int		rs = SR_OK ;
	for (cauto &m : mfuns) {
	    rs = (this->*m)() ;
	    if (rs) break ;
	} /* end for */
	return rs ;
} /* end method (execnamer::operator) */

int execnamer::tryargz() noex {
    	int		rs = SR_OK ;
	if ((efn == nullptr) || (*efn == eos)) {
	    if (pip->argz[0] == '/') {
		efn = pip->argz ;
		rs = lenstr(efn) ;
	    } /* end if (got from ARGZ) */
	} /* end if (needed) */
	return rs ;
} /* end method (execnamer::tryargz) */

int execnamer::tryshell() noex {
    	int		rs = SR_OK ;
	if ((efn == nullptr) || (*efn == eos)) {
	    if (cchar *varp = getourenv(pip->envv,VARUNDER)) {
		if (ccharp cp = getunderpath(varp)) ylikely {
		    if (*cp) {
		        efn = cp ;
		        rs = lenstr(efn) ;
		    } /* end if (non-empty) */
		} /* end if (getunderpath) */
	    } /* end if (getoutenv) */
	} /* end if (needed) */
	return rs ;
} /* end method (execnamer::tryshell) */

int execnamer::tryenvfn() noex {
    	int		rs = SR_OK ;
	if ((efn == nullptr) || (*efn == eos)) {
	    if ((efn = getourenv(pip->envv,VAREXECFNAME)) ; efn) {
		rs = lenstr(efn) ;
	    } /* end if (getoutenv) */
	} /* end if (needed) */
	return rs ;
} /* end method (execnamer::tryenvfn) */

int execnamer::trysys() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if ((efn == nullptr) || (*efn == eos)) {
	    if (char *fbuf ; (rs = mem.mp(&fbuf)) >= 0) ylikely {
	        if ((rs = u_execname(fbuf,rs)) > 0) ylikely {
		    rs = pip->setent(&efn,fbuf,rs) ;
		    len = rs ;
	        } /* end if (u_execname) */
	        rs1 = mem.free(fbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (needed) */
	return (rs >= 0) ? len : rs ;
} /* end method (execnamer::trysys) */

local ccharp getunderpath(ccharp varp) noex  {
    	ccharp rp = nullptr ;
	if (varp) ylikely {
	    if (ccp tp = strochr(varp,chx_star)) ylikely {
		if (tp = strochr((tp + 1),chx_star) ; tp) ylikely {
		    rp = (tp + 1) ;
		} /* end if (second star) */
	    } else {
		rp = varp ;
	    } /* end if (first star) */
	} /* end if (non-null) */
    	return rp ;
} /* end subroutine (getunderpath) */


