/* gethz SUPPORT */
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
				1 -> 'HZ' define only
				2 -> environment variable 'HZ'
				3 -> 'CLK_TCK' define (or facility) only
				4 -> |sysconf(3c)| 'CLK_TCK' only
				4 -> guess value

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
#include	<sys/param.h>		/* |HZ| for those that have it */
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>		/* |getenv(3c)| + |atoi(3c)| */
#include	<usystem.h>
#include	<cfdec.h>
#include	<isnot.h>		/* |isNotValid(3uc)| */
#include	<localmisc.h>

#include	"gethz.h"


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

#ifndef	VARHZ
#define	VARHZ		"HZ"
#endif

#define	HZ_GUESS	100		/* guessed value */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct hzmgr ;
    typedef int (hzmgr::*hzmgr_m)(int) noex ;
    struct hzmgr {
	int		hz ;
	int operator () (int) noex ;
	int getval(int) noex ;
	int tryconst(int) noex ;
	int tryenv(int) noex ;
	int trytck(int) noex ;
	int tryconf(int) noex ;
	int tryguess(int) noex ;
    } ; /* end struct (hzmgr) */
}

constexpr hzmgr_m	mems[] = {
	&hzmgr::tryconst,
	&hzmgr::tryenv,
	&hzmgr::trytck,
	&hzmgr::tryconf,
	&hzmgr::tryguess
} ;


/* forward references */

static int	decval(cchar *) noex ;


/* local variables */

static hzmgr		gethz_data ;


/* exported variables */


/* exported subroutines */

int gethz(int w) noex {
	return gethz_data(w) ;
}
/* end subroutine (gethz) */


/* local subroutines */

int hzmgr::operator () (int w) noex {
	int		rs = SR_OK ;
	if ((hz == 0) || (w > 0)) {
	    rs = getval(w) ;
	} else {
	    rs = hz ;
	}
	return rs ;
}
/* end method (hzmgr::operator) */

int hzmgr::getval(int w) noex {
	int		rs = SR_OK ;
	for (auto &m : mems) {
	    rs = (this->*m)(w) ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
}
/* end method (hzmgr::getval) */

int hzmgr::tryconst(int w) noex {
	int		rs = SR_OK ;
	if ((hz == 0) && ((w == gethz_any) || (w == gethz_const))) {
	    rs = HZ ;
	    hz = rs ;
	}
	return rs ;
}
/* end method (hzmgr::tryconst) */

int hzmgr::tryenv(int w) noex {
	int		rs = SR_OK ;
	if ((hz == 0) && ((w == gethz_any) || (w == gethz_env))) {
	    static cchar	*val = getenv(VARHZ) ;
	    if (val) {
	        if ((rs = decval(val)) > 0) {
	            hz = rs ;
		} else if (isNotValid(rs)) {
		    rs = SR_OK ;
		}
	    }
	}
	return rs ;
}
/* end method (hzmgr::tryenv) */

int hzmgr::trytck(int w) noex {
	int		rs = SR_OK ;
	if ((hz == 0) && ((w == gethz_any) || (w == gethz_tck))) {
	    rs = CLK_TCK ;
	    hz = rs ;
	}
	return rs ;
}
/* end method (hzmgr::trytck) */

int hzmgr::tryconf(int w) noex {
	int		rs = SR_OK ;
	if ((hz == 0) && ((w == gethz_any) || (w == gethz_conf))) {
	    if (int req ; (req = _SC_CLK_TCK) >= 0) {
	        if ((rs = uc_sysconfval(req,nullptr)) >= 0) {
		    hz = rs ;
	        } else if (isNotValid(rs) || isNotSupport(rs)) {
		    rs = SR_OK ;
		}
	    }
	}
	return rs ;
}
/* end method (hzmgr::tryconf) */

int hzmgr::tryguess(int w) noex {
	int		rs = SR_OK ;
	if ((hz == 0) && ((w == gethz_any) || (w == gethz_guess))) {
	    rs = HZ_GUESS ;
	}
	return rs ;
}
/* end method (hzmgr::tryguess) */

static int decval(cchar *s) noex {
	int		rs ;
	int		rv{} ;
	if ((rs = cfdeci(s,-1,&rv)) >= 0) {
	    if (rv < 0) rs = SR_DOM ;
	}
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (decval) */


