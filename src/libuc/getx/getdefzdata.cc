/* getdefzdata SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide an OS-independent time management operation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getdefzdata

	Description:
	We return some default time-zone data (name and other).

	Sysnopsis:
	int getdefzdata(defzdata *zip,char *zbuf,int zlen,int isdst) noex

	Arguments:
	zip		DEFZDATA object pointer
	zbuf		time-zone-name abbreviation result buffer pointer
	zlen		time-zone-name abbreviation result buffer length
	isdst		is-daylight-savings-time value

	Returns:
	>=0		length of result buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* <- for |time(2)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usysflag.h>
#include	<bufsizevar.hh>		/* |zn| */
#include	<snwcpy.h>
#include	<altzone.h>
#include	<localmisc.h>

#include	"getdefzdata.h"


/* local defines */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	znlen(getbufsize_zn) ;	/* time-zone-name (abbr) */

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int getdefzdata(defzdata *zip,char *zbuf,int zlen,int isdst) noex {
	int		rs = SR_FAULT ;
	int		znl = 0 ;
	if (zip && zbuf) {
	    cchar	*zp{} ;
	    if_constexpr (f_darwin) {
		custime		dt = time(nullptr) ;
		TM		tmo ;
	 	if ((rs = uc_localtime(&dt,&tmo)) >= 0) {
	            zip->zoff = intconv(tmo.tm_gmtoff / 60) ;
	            zp = tmo.tm_zone ;
		}
	    } else {
		rs = SR_OK ;
	        tzset() ;
		{
	            bool f_daylight = (isdst >= 0) ? isdst : daylight ;
	            zip->zoff = (((f_daylight) ? altzone : timezone) / 60) ;
	            zp = (f_daylight) ? tzname[1] : tzname[0] ;
		}
	    } /* end if_constexpr (f_darwin) */
	    if ((rs >= 0) && zp) {
		if ((rs = znlen) >= 0) {
	            rs = snwcpy(zbuf,zlen,zp,rs) ;
		    znl = rs ;
		}
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? znl : rs ;
}
/* end subroutine (getdefzdata) */

int defzdata::get(char *zbuf,int zlen,int dst) noex {
	return getdefzdata(this,zbuf,zlen,dst) ;
}


