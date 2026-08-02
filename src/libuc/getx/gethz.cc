/* gethz SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the machine HZ */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This is a spin off of various programs that needed to get
	the machine HZ value.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	gethz

	Description:
	Get the system statistics-clock frequency ("HZ").

	Synopsis:
	int gethz(int w) noex

	Arguments:
	w		which source to use:
				0 -> any
				1 -> environment variable 'HZ'
				2 -> |sysconf(3c)| 'CLK_TCK' only
				3 -> 'HZ' define only
				4 -> 'CLK_TCK' define (or facility) only
				5 -> guess value

	Returns:
	>0		HZ value
	==0		could not be determined
	<0		error code (system-return)

	Notes:
	1. In the old days, people actually cared about what this value
	was.  That no longer seems to be the case.
	2. This is fairly platform (OS) independent even without special
	operating system support.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>		/* POSIX |HZ| for those that have it */
#include	<unistd.h>		/* POSIX */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucsysconf.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC |isNotValid(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"gethz.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;

/* local defines */

#ifndef	HZ
#define	HZ		0
#endif

#ifndef	CLK_TCK
#define	CLK_TCK		0
#endif

#ifndef	_SC_CLK_TCK
#define	_SC_CLK_TCK	-1
#endif

#define	HZ_GUESS	100		/* guessed value (AT&T 3B2 series) */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct hzmgr ;
    typedef int (hzmgr::*hzmgr_m)(gethzes) noex ;
    struct hzmgr {
	int		hz ;
	int operator () (gethzes) noex ;
	bool isUs	(gethzes,gethzes) noex ;
	int getany	(gethzes) noex ;
	int tryenv	(gethzes) noex ;
	int tryconf	(gethzes) noex ;
	int tryconst	(gethzes) noex ;
	int trytck	(gethzes) noex ;
	int tryguess	(gethzes) noex ;
    } ; /* end struct (hzmgr) */
} /* end namespace */

constexpr hzmgr_m	tries[] = {
	&hzmgr::tryenv,
	&hzmgr::tryconf,
	&hzmgr::tryconst,
	&hzmgr::trytck,
	&hzmgr::tryguess
} ; /* end array (tries) */


/* forward references */

local int	decval(cchar *) noex ;


/* local variables */

static hzmgr		gethz_data ;


/* exported variables */

ucgetx::gethzer		syshz ;


/* exported subroutines */

namespace ucgetx {
    int gethzer::operator () (gethzes w) noex {
    	return gethz_data(w) ;
    } /* end method (gethzer::operator) */
} /* end namespace (ucgetx) */

int gethz(int wi) noex {
    	gethzes	w = gethzes(wi) ;
    	return gethz_data(w) ;
} /* end subroutine (gethz) */


/* local subroutines */

int hzmgr::operator () (gethzes w) noex {
	int		rs = SR_OK ;
	if ((hz == 0) || (w > 0)) {
	    rs = getany(w) ;
	} else {
	    rs = hz ;
	}
	return rs ;
} /* end method (hzmgr::operator) */

bool hzmgr::isUs(gethzes wme,gethzes w) noex {
    	return ((hz == 0) && ((w == gethz_any) || (w == wme))) ;
} /* end method (hdmgr::isUs) */

int hzmgr::getany(gethzes w) noex {
	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)(w) ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
} /* end method (hzmgr::getany) */

int hzmgr::tryenv(gethzes w) noex {
	int		rs = SR_OK ;
	if (isUs(gethz_env,w)) {
	    static cchar *val = getenver(varname.hz) ;
	    if (val) {
	        if ((rs = decval(val)) > 0) {
	            hz = rs ;
		} else if (isNotValid(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (HZ environment) */
	} /* end if (isUs) */
	return rs ;
} /* end method (hzmgr::tryenv) */

int hzmgr::tryconf(gethzes w) noex {
	int		rs = SR_OK ;
	if (isUs(gethz_conf,w)) {
	    if (cint cmd = _SC_CLK_TCK ; cmd >= 0) {
	        if ((rs = uc_sysconfval(cmd,nullptr)) >= 0) {
		    hz = rs ;
	        } else if (isNotValid(rs) || isNotSupport(rs)) {
		    rs = SR_OK ;
		}
	    }
	} /* end if (isUs) */
	return rs ;
} /* end method (hzmgr::tryconf) */

int hzmgr::tryconst(gethzes w) noex {
	int		rs = SR_OK ;
	if (isUs(gethz_const,w)) {
	    rs = HZ ;
	    hz = rs ;
	} /* end if (isUs) */
	return rs ;
} /* end method (hzmgr::tryconst) */

int hzmgr::trytck(gethzes w) noex {
	int		rs = SR_OK ;
	if (isUs(gethz_tck,w)) {
	    rs = CLK_TCK ;
	    hz = rs ;
	} /* end if (isUs) */
	return rs ;
} /* end method (hzmgr::trytck) */

int hzmgr::tryguess(gethzes w) noex {
	int		rs = SR_OK ;
	if (isUs(gethz_guess,w)) {
	    rs = HZ_GUESS ;
	} /* end if (isUs) */
	return rs ;
} /* end method (hzmgr::tryguess) */

local int decval(cchar *s) noex {
	int		rs ;
	int		rv = 0 ; /* return-value */
	if ((rs = cfdeci(s,-1,&rv)) >= 0) ylikely {
	    if (rv < 0) rs = SR_DOM ;
	}
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (decval) */


