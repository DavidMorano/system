/* cvtdater SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* date conversion object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-23, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cvtdater

	Description:
	This small object assists in converting text strings
	representing dates into UNIX® time values.  The hard work
	is actually done by the included TMZ object.

	Source formats include:
	  YYMMDD
	CCYYMMDD

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<dayspec.h>
#include	<tmz.hh>
#include	<tmtime.hh>
#include	<hasx.h>		/* |hasalpha(3uc)| */
#include	<localmisc.h>

#include	"cvtdater.h"

import libutil ;

/* local defines */


/* external subroutines */


/* local subroutines */


/* forward references */

static int	cvtdater_daytime(cvtdater *,time_t * = nullptr) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cvtdater_start(cvtdater *op,time_t dt) noex {
	CVTDATER	*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    if (dt == 0) dt = getustime ;
	    rs = memclear(hop) ;
	    op->daytime = dt ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cvtdater_start) */

int cvtdater_load(cvtdater *op,time_t *dp,cchar *cp,int cl) noex {
	int		rs = SR_FAULT ;
	if (op && cp) {
	    tmz		stz ;
	    if (cl < 0) cl = xstrlen(cp) ;
	    if (hasalpha(cp,cl)) {
	        tmz_init(&stz) ;
	        if (dayspec ds ; (rs = ds.load(cp,cl)) >= 0) {
		    rs = stz.setday(ds.y,ds.m,ds.d) ;
	        }
	    } else {
	        rs = stz.xday(cp,cl) ;
	    }
	    if (rs >= 0) {
	        tmtime	tmt ;
	        if (stz.hasyear == 0) {
	            cvtdater_daytime(op) ;	/* get current date */
	            if ((rs = tmt.localtime(op->daytime)) >= 0) {
		        rs = stz.setyear(tmt.year) ;
		    }
	        } /* end if (getting the current year) */
	        if (rs >= 0) {
	            if ((rs = tmt.insert(&stz.st)) >= 0) {
	                if (time_t t{} ; (rs = tmt.mktime(&t)) >= 0) {
	                    if (dp) *dp = t ;
		        }
		    }
	        } /* end if (ok) */
	    } /* end if (ok - TMZ scope) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cvtdater_load) */

int cvtdater_finish(cvtdater *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cvtdater_finish) */


/* private subroutines */

static int cvtdater_daytime(cvtdater *op,time_t *rp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->daytime == 0) {
		op->daytime = getustime ;
	    }
	    if (rp) *rp = op->daytime ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cvtdater_daytime) */

int cvtdater::start(time_t t) noex {
	return cvtdater_start(this,t) ;
}

int cvtdater::load(time_t *tp,cchar *sp,int sl) noex {
	return cvtdater_load(this,tp,sp,sl) ;
}

void cvtdater::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("cvtdater",rs,"fini-finish") ;
	}
}

cvtdater_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case cvtdatermem_finish:
	        rs = cvtdater_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (cvtdater_co::operator) */


