/* cvtdater */

/* date conversion object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-23, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This small object assists in converting text strings
	representing dates into UNIX® time values. The hard work
	is actually done by the included TMZ object.

	Source formats include:

	  YYMMDD
	CCYYMMDD

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<dayspec.h>
#include	<tmz.h>
#include	<tmtime.h>
#include	<localmisc.h>

#include	"cvtdater.h"


/* local defines */


/* external subroutines */


/* local subroutines */


/* forward references */

static int	cvtdater_daytime(CVTDATER *,time_t *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cvtdater_start(CVTDATER *cdp,time_t daytime) noex {
	if (cdp == nullptr) return SR_FAULT ;

	memset(cdp,0,sizeof(CVTDATER)) ;
	cdp->daytime = daytime ;

	return SR_OK ;
}
/* end subroutine (cvtdater_start) */

int cvtdater_load(CVTDATER *cdp,time_t *dp,cchar *cp,int cl) noex {
	TMZ		stz ;
	int		rs ;

	if (cdp == nullptr) return SR_FAULT ;
	if (cp == nullptr) return SR_FAULT ;

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
	        cvtdater_daytime(cdp,nullptr) ;	/* get current date */
	        rs = tmtime_localtime(&tmt,cdp->daytime) ;
		tmz_setyear(&stz,tmt.year) ;
	    } /* end if (getting the current year) */

	    if (rs >= 0) {
	        time_t	t ;
	        tmtime_insert(&tmt,&stz.st) ;
	        if ((rs = tmtime_mktime(&tmt,&t)) >= 0) {
	            if (dp != nullptr) *dp = t ;
		}
	    } /* end if (ok) */

	} /* end if (tmz_day) */

	return rs ;
}
/* end subroutine (cvtdater_load) */

int cvtdater_finish(CVTDATER *cdp) noex {
	int		rs = SR_FAULT ;
	if (cdp) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cvtdater_finish) */


/* private subroutines */

static int cvtdater_daytime(CVTDATER *cdp,time_t *rp) noex {
	int		rs = SR_FAULT ;
	if (cdp) {
	    rs = SR_OK ;
	    if (cdp->daytime == 0) cdp->daytime = time(nullptr) ;
	    if (rp) *rp = cdp->daytime ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cvtdater_daytime) */


