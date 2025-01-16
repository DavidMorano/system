/* date SUPPORT */
/* encoding=ISO8859-1 */
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
 
  	Object:
	date

	Description:
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<bufsizevar.hh>		/* for |za| */
#include	<mallocxx.h>
#include	<snwcpy.h>
#include	<strn.h>		/* |strnwcpy(3uc)| */
#include	<sncpyx.h>
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


/* local variables */

static bufsizevar		znlen(getbufsize_za) ;


/* exported variables */


/* exported subroutines */

int date_start(date *op,time_t t,int zoff,int isdst,cchar *zbuf,int zlen) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;
	    if (char *a ; (rs = malloc_za(&a)) >= 0) {
		cint	znlen = rs ;
		a[znlen] = '\0' ;
		op->zname = a ;
	        op->time = t ;
	        op->zoff = zoff ;
	        op->isdst = isdst ;
		if (zbuf) {
	            rs = (strnwcpy(op->zname,znlen,zbuf,zlen) - op->zname) ;
		}
		if (rs < 0) {
		    uc_free(op->zname) ;
		    op->zname = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (date_start) */

int date_finish(date *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->zname) {
		rs1 = uc_free(op->zname) ;
		if (rs >= 0) rs = rs1 ;
		op->zname = nullptr ;
	    }
	    op->time = 0 ;
	}
	return rs ;
}
/* end subroutine (date_finish) */

int date_setzname(date *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    rs = (strnwcpy(op->zname,znlen,sp,sl) - op->zname) ;
	}
	return rs ;
}
/* end subroutine (date_setzname) */

int date_copy(date *op,date *d2p) noex {
	int		rs = SR_FAULT ;
	if (op && d2p) {
	    char	*znp = op->zname ;	/* save */
	    rs = SR_OK ;
	    memcpy(op,d2p) ;
	    op->zname = znp ;			/* restore */
	}
	return rs ;
}
/* end subroutine (date_copy) */

int date_gettime(date *op,time_t *tp) noex {
	int		rs = SR_FAULT ;
	if (op && tp) {
	    rs = SR_OK ;
	    *tp = op->time ;
	}
	return rs ;
}
/* end subroutine (date_gettime) */

int date_getzoff(date *op,int *zop) noex {
	int		rs = SR_FAULT ;
	if (op && zop) {
	    rs = SR_OK ;
	    *zop = op->zoff ;
	}
	return rs ;
}
/* end subroutine (date_getzoff) */

int date_getisdst(date *op,int *dstp) noex {
	int		rs = SR_FAULT ;
	if (op && dstp) {
	    rs = SR_OK ;
	    *dstp = int(op->isdst) ;
	}
	return rs ;
}
/* end subroutine (date_getisdst) */

int date_getzname(date *op,char *zbuf,int zlen) noex {
	int		rs = SR_FAULT ;
	if (op && zbuf) {
	    rs = sncpy(zbuf,zlen,op->zname) ;
	}
	return rs ;
}
/* end subroutine (date_getzname) */


