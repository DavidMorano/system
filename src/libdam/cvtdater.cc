/* cvtdater SUPPORT */
/* encoding=ISO8859-1 */
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
	representing dates into UNIX® time values. The hard work
	is actually done by the included TMZ object.

	Source formats include:

	  YYMMDD
	CCYYMMDD

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<dayspec.h>
#include	<tmz.h>
#include	<tmtime.h>
#include	<hasx.h>		/* |hasalpha(3uc)| */
#include	<localmisc.h>

#include	"cvtdater.h"


/* local defines */


/* external subroutines */


/* local subroutines */


/* forward references */

static int	cvtdater_daytime(cvtdater *,time_t *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cvtdater_start(cvtdater *op,time_t daytime) noex {
	CVTDATER	*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(hop) ;
	    op->daytime = daytime ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cvtdater_start) */

int cvtdater_load(cvtdater *op,time_t *dp,cchar *cp,int cl) noex {
	int		rs = SR_FAULT ;
	if (op && cp) {
	    TMZ		stz ;
	    if (hasalpha(cp,cl)) {
	        DAYSPEC	ds ;
	        tmz_init(&stz) ;
	        if ((rs = dayspec_load(&ds,cp,cl)) >= 0) {
		    rs = tmz_setday(&stz,ds.y,ds.m,ds.d) ;
	        }
	    } else {
	        rs = tmz_day(&stz,cp,cl) ;
	    }
	    if (rs >= 0) {
	        TMTIME	tmt ;
	        if (tmz_hasyear(&stz) == 0) {
	            cvtdater_daytime(op,nullptr) ;	/* get current date */
	            rs = tmtime_localtime(&tmt,op->daytime) ;
		    tmz_setyear(&stz,tmt.year) ;
	        } /* end if (getting the current year) */
	        if (rs >= 0) {
	            if ((rs = tmtime_insert(&tmt,&stz.st)) >= 0) {
	                if (time_t t{} ; (rs = tmtime_mktime(&tmt,&t)) >= 0) {
	                    if (dp) *dp = t ;
		        }
		    }
	        } /* end if (ok) */
	    } /* end if (tmz_day) */
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


