/* pcsgetdate SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* parse a date string into its componenent values */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-02-01, David A­D­ Morano
	This subroutine parses a date string, of a specified type,
	into its componenent values.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
  	Name:
	pcsgetdate

	Description:
	This subroutine parses a date-string of some sort into its
	parsed out coponment values (stored in a TMTIME object).

	Synopsis:
	int pcsgetdate(cc *pr,tmtime *tmp,cc *sp,int sl,pcsdatætypes dt) noex

	Arguments:
	pr		PCS program-root
	tmp		TMTIME object pointer to receive results
	sp		source date-string pointer
	sl		source date-string length
	dt		date-type of date-string to convert:
				0= envelope
				1= message

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<tmtime.hh>
#include	<tmz.hh>
#include	<localmisc.h>

#include	"pcsgetdate.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int tmz_typer(tmz *zp,cc *sp,int sl,pcsdatetypes dt) noex {
    	int		rs ;
	switch (dt) {
	case pcsdatetype_env:
	    rs = zp->xstd(sp,sl) ;
	    break ;
	case pcsdatetype_msg:
	    rs = zp->xmsg(sp,sl) ;
	    break ;
	default:
	    rs = SR_NOTSUP ;
	    break ;
	} /* end switch */
	return rs ;
} /* end subroutine (tmz_typer) */

local int tmtime_load(tmtime *,tmz *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsgetdate(cchar *pr,tmtime *tmp,cc *sp,int µsl,pcsdatetypes dt) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (pr && tmp) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	        rs = SR_INVALID ;
	        if (pr[0] && (dt >= 0)) {
		    if (tmz z ; (rs = z.init) >= 0) {
			if ((rs = tmz_typer(&z,sp,sl,dt)) >= 0) {
			    rs = tmtime_load(tmp,&z) ;
			    rv = rs ;
			} /* end if (tmz_typer) */
			rs1 = z.fini ;
	        	if (rs >= 0) rs = rs1 ;
		    } /* end if (tmz) */
	        } /* end if (valid) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (pcsgetdate) */


/* local subroutines */

local int tmtime_loadzoff(tmtime *tmp,tmz *zp) noex {
    	int		rs ;
	if ((rs = zp->haszoff) > 0) {
	    if (short zoff ; (rs = zp->getzoff(&zoff)) > 0) {
		/* conversion of minutes to seconds (west of GMY) */
	        tmp->gmtoff = (zoff * 60) ;
	    }
	} /* end if (tmz_haszoff) */
	return rs ;
} /* end subroutine (tmtime_loadzoff) */

local int tmtime_loadzone(tmtime *tmp,tmz *zp) noex {
    	int		rs ;
	int		rs1 ;
	int		zl = 0 ; /* return-value */
	if ((rs = zp->haszone) > 0) {
	    rs = SR_NOTOPEN ;
	    if (zp->zname) {
	        if (char *zbuf ; (rs = lm_zn(&zbuf)) >= 0) {
		    cint zlen = rs ;
		    if ((rs = zp->getzone(zbuf,zlen)) >= 0) {
		        rs = tmp->loadzn(zbuf,rs) ;
			zl = rs ;
		    } /* end if (tmz_getzone) */
		    rs1 = lm_free(zbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (zone-name available) */
	} /* end if (tmz_haszone) */
	return (rs >= 0) ? zl : rs ;
} /* end subroutine (tmtime_loadzone) */

local int tmtime_load(tmtime *tmp,tmz *zp) noex {
    	int		rs ;
	int		rv = 0 ; /* return-value */
	if (TM vals{} ; (rs = zp->gettm(&vals)) >= 0) {
	    if ((rs = tmp->insert(&vals)) >= 0) {
	        if ((rs = tmtime_loadzoff(tmp,zp)) >= 0) {
		    rs = tmtime_loadzone(tmp,zp) ;
		    rv = rs ;
		}
	    } /* end if (tmtime_insert) */
	} /* end if (tmz_gettm) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (tmtime_load) */


