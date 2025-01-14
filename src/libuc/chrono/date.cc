/* date SUPPORT */
/* lang=C++20 */

/* general date object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	Originally created due to frustration with various other
	"fuzzy" date conversion subroutines.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
	This object can be used to create dates from various input
	data including strings.

	Note:
	The timezone offset value in 'struct timeb' is the minutes
	west of GMT.  This is a positive value for timezones that
	are west of Greenwich. It is negative for timezones east
	of Greenwich. This is opposite from what you will see in
	email headers (for example). Our number here must be
	subtracted from GMT in order to get the local time.

	Frustration note:
	What an incredible pain this time-date handling stuff all
	is? This file does not even do justice to a small fraction
	of the real problems associated with data management!  The
	problem is that the date changes as time progresses.  Changes
	are due to timezone differences and year leaps and leap
	seconds.  The fact that timezone data is not a part of many
	dates only complicates matters worse as we then have to try
	and figure out a reasonable timezone when they are not
	supplied.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/timeb.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<snwcpy.h>
#include	<strn.h>		/* |strnncpy(3uc)| */
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"date.h"
#include	"tmz.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local (static) variables */

constexpr int		znamelen = DATE_ZNAMELEN ;


/* exported variables */


/* exported subroutines */

int date_start(date *dp,time_t t,int zoff,int isdst,cchar *zbuf,int zlen) noex {
	int		rs = SR_FAULT ;
	if (dp && zbuf) {
	    memclear(dp) ;
	    dp->time = t ;
	    dp->zoff = zoff ;
	    dp->isdst = isdst ;
	    rs = (strnncpy(dp->zname,zbuf,zlen,znamelen) - dp->zname) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (date_start) */

int date_finish(date *dp) noex {
	int		rs = SR_FAULT ;
	if (dp) {
	    dp->time = 0 ;
	}
	return rs ;
}
/* end subroutine (date_finish) */

int date_copy(date *dp,date *d2p) noex {
	int		rs = SR_FAULT ;
	if (dp && d2p) {
	    memcpy(dp,d2p) ;
	}
	return rs ;
}
/* end subroutine (date_copy) */

int date_gettime(date *dp,time_t *tp) noex {
	int		rs = SR_FAULT ;
	if (dp && tp) {
	    *tp = dp->time ;
	}
	return rs ;
}
/* end subroutine (date_gettime) */

int date_getzoff(date *dp,int *zop) noex {
	int		rs = SR_FAULT ;
	if (dp && zop) {
	    *zop = dp->zoff ;
	}
	return rs ;
}
/* end subroutine (date_getzoff) */

int date_getisdst(date *dp,int *dstp) noex {
	int		rs = SR_FAULT ;
	if (dp && dstp) {
	    *dstp = int(dp->isdst) ;
	}
	return rs ;
}
/* end subroutine (date_getisdst) */

int date_getzname(date *dp,char *zbuf,int zlen) noex {
	int		rs = SR_FAULT ;
	if (dp && zbuf) {
	    rs = snwcpy(zbuf,zlen,dp->zname,znamelen) ;
	}
	return rs ;
}
/* end subroutine (date_getzname) */


