/* date SUPPORT */
/* charset=ISO8859-1 */
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
#include	<sys/timeb.h>		/* |TIMEB| */
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
#include	"tmz.hh"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar		znlen(getbufsize_zn) ;


/* exported variables */


/* exported subroutines */

int date_start(date *op,time_t t,int zoff,int isdst,cchar *zbuf,int zlen) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;
	    if ((rs = znlen) >= 0) {
	        if (char *a ; (rs = malloc_zn(&a)) >= 0) {
		    a[znlen] = '\0' ;
		    op->zname = a ;
	            op->time = t ;
	            op->zoff = shortconv(zoff) ;
	            op->isdst = shortconv(isdst) ;
		    if (zbuf) {
		        char *zp = op->zname ; /* white "buffer" */
	                rs = intconv(strnwcpy(zp,znlen,zbuf,zlen) - zp) ;
		    }
		    if (rs < 0) {
		        uc_free(op->zname) ;
		        op->zname = nullptr ;
		    }
	        } /* end if (memory-allocation) */
	    } /* end if (znlen) */
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
	    char	*zp = op->zname ; /* "write" buffer */
	    rs = intconv(strnwcpy(zp,znlen,sp,sl) - zp) ;
	}
	return rs ;
}
/* end subroutine (date_setzname) */

int date_copy(date *op,date *d2p) noex {
	int		rs = SR_FAULT ;
	if (op && d2p) {
	    DATE	*hop = op ;
	    char	*znp = op->zname ;	/* save */
	    rs = SR_OK ;
	    memcpy(hop,d2p) ;			/* shallow-copy */
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


/* local subroutines */

int date::start(time_t t,int off,int dst,cchar *zbuf,int zlen) noex {
	return date_start(this,t,off,dst,zbuf,zlen) ;
}

int date::setzname(cchar *sp,int sl) noex {
	return date_setzname(this,sp,sl) ;
}

int date::copy(date *d2p) noex {
	return date_copy(this,d2p) ;
}

int date::gettime(time_t *tp) noex {
	return date_gettime(this,tp) ;
}

int date::getzoff(int *zop) noex {
	return date_getzoff(this,zop) ;
}

int date::getisdst(int *dstp) noex {
	return date_getisdst(this,dstp) ;
}

int date::getzname(char *zbuf,int zlen) noex {
	return date_getzname(this,zbuf,zlen) ;
}

void date::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("date",rs,"fini-finish") ;
	}
}

date_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case datemem_finish:
	        rs = date_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (date_co::operator) */


