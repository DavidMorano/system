/* dater_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* general dater object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	Originally created due to frustration with various other
	"fuzzy" dater conversion subroutines.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
  	Object:
	dater

  	Description:
	This object can be used to create daters from various input
	data including strings.

	Note:
	The timezone offset value in 'TIMEB' is the minutes
	west of GMT.  This is a positive value for timezones that
	are west of Greenwich.  It is negative for timezones east
	of Greenwich.  This is opposite from what you will see in
	email headers (for example).  Our number here must be
	subtracted from GMT in order to get the local time.

	Frustration note:
	What an incredible pain this time-dater handling stuff all
	is?  This file does not even do justice to a small fraction
	of the real problems associated with dater management!  The
	problem is that the dater changes as time progresses.
	Changes are due to timezone differences and year leaps and
	leap seconds.  The fact that timezone data is not a part
	of many daters only complicates matters worse as we then
	have to try and figure out a reasonable timezone when they
	are not supplied.

	Comment parseing for RFC-822 daters:
	Note that daters given to us for MSG (RFC-822) processing,
	might have comments in them.  These comments are those
	specified in RFC-822.  Note that the tmz object does some
	comment processing on MSG daters but only in the same way
	that NetNews does ; namely, only for a comment at the end
	of the string and which contains a time-zone-name.

	Full comment parsing is done on MSG daters using a COMPARSE
	object.  With COMPARSE processing, we still try to divine a
	time-zone-name from the leading part of the resulting
	comment.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/timeb.h>		/* |TIMEB| */
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<ctime>			/* |time_t| */
#include	<climits>		/* |SHORT_MIN| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<uctimeconv.h>		/* |uc_mktime(3uc)| */
#include	<getdefzdata.h>
#include	<bufsizevar.hh>
#include	<calstrs.h>
#include	<sbuf.h>
#include	<tmtime.hh>		/* CHRONO */
#include	<zos.h>			/* CHRONO */
#include	<comparse.h>		/* CHRONO */
#include	<tmz.hh>		/* CHRONO */
#include	<zdb.h>			/* CHRONO */
#include	<sncpyx.h>
#include	<snwcpyx.h>
#include	<sntmtime.h>
#include	<strn.h>		/* |strnwcpyxc(3uc)| */
#include	<strwcpy.h>
#include	<snwcpy.h>
#include	<mkchar.h>
#include	<hasx.h>		/* |hasalldig(3uc)| */
#include	<ischarx.h>		/* |isdigitlatin(3uc)| */
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"dater.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	TZO_EMPTY
#define	TZO_EMPTY	SHORT_MIN
#endif

#ifndef	TZO_MAXZOFF
#define	TZO_MAXZOFF	(14*60)
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dater::start(TIMEB *nowp,cchar *zsp,int zsl) noex {
	return dater_start(this,nowp,zsp,zsl) ;
}

int dater::startcopy(dater *d2p) noex {
	return dater_startcopy(this,d2p) ;
}

int dater::setcopy(dater *d2p) noex {
	return dater_setcopy(this,d2p) ;
}

int dater::setstd(cchar *sp,int sl) noex {
	return dater_setstd(this,sp,sl) ;
}

int dater::setmsg(cchar *sp,int sl) noex {
	return dater_setmsg(this,sp,sl) ;
}

int dater::setstrdig(cchar *sp,int sl) noex {
	return dater_setstrdig(this,sp,sl) ;
}

int dater::setlogz(cchar *sp,int sl) noex {
	return dater_setlogz(this,sp,sl) ;
}

int dater::settouch(cchar *sp,int sl) noex {
	return dater_settouch(this,sp,sl) ;
}

int dater::settoucht(cchar *sp,int sl) noex {
	return dater_settoucht(this,sp,sl) ;
}

int dater::settmzon(TM *stp,int zoff,cchar *zstr,int zlen) noex {
	return dater_settmzon(this,stp,zoff,zstr,zlen) ;
}

int dater::settmzo(TM *stp,int zoff) noex {
	return dater_settmzo(this,stp,zoff) ;
}

int dater::settmzn(TM *stp,cchar *zstr,int zlen) noex {
	return dater_settmzn(this,stp,zstr,zlen) ;
}

int dater::settimezn(time_t t,cchar *zn,int isdst) noex {
	return dater_settimezn(this,t,zn,isdst) ;
}

int dater::settimezon(time_t t,int zoff,cchar *zn,int isdst) noex {
	return dater_settimezon(this,t,zoff,zn,isdst) ;
}

int dater::mkdatestr(daterdts dt,char *dbuf,int dlen) noex {
	return dater_mkdatestr(this,dt,dbuf,dlen) ;
}

int dater::mkstd(char *dbuf,int dlen) noex {
	return dater_mkstd(this,dbuf,dlen) ;
}

int dater::mkenv(char *dbuf,int dlen) noex {
	return dater_mkenv(this,dbuf,dlen) ;
}

int dater::mkmsg(char *dbuf,int dlen) noex {
	return dater_mkmsg(this,dbuf,dlen) ;
}

int dater::mkhdr(char *dbuf,int dlen) noex {
	return dater_mkhdr(this,dbuf,dlen) ;
}

int dater::mkstrdig(char *dbuf,int dlen) noex {
	return dater_mkstrdig(this,dbuf,dlen) ;
}

int dater::mklogz(char *dbuf,int dlen) noex {
	return dater_mklogz(this,dbuf,dlen) ;
}

int dater::mkgmtlogz(char *dbuf,int dlen) noex {
	return dater_mkgmtlogz(this,dbuf,dlen) ;
}

int dater::zinfoset(dater_zi *zip,cchar *zn,int zl) noex {
	return dater_zinfoset(this,zip,zn,zl) ;
}

int dater::gettime(time_t *tp) noex {
	return dater_gettime(this,tp) ;
}

int dater::getzoneoff(int *zp) noex {
	return dater_getzoneoff(this,zp) ;
}

int dater::getzonename(char *rbuf,int rlen) noex {
	return dater_getzonename(this,rbuf,rlen) ;
}

int dater::zinfoget(dater_zi *zip,char *zbuf,int zlen) noex {
	return dater_zinfoget(this,zip,zbuf,zlen) ;
}

int dater::diff(dater *d2p,time_t *rp) noex {
	return dater_diff(this,d2p,rp) ;
}

void dater::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("dater",rs,"fini-finish") ;
	}
} /* end method (dater::dtor) */

dater_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case datermem_finish:
	        rs = dater_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (dater_co::operator) */


