/* getdefzinfo SUPPORT */
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
	getdefzinfo

	Description:
	We return some default time-zone information.

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

#include	"getdefzinfo.h"


/* local defines */


/* external subroutines */


/* external varaibles */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	znlen(getbufsize_zn) ;

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int getdefzinfo(defzinfo *zip,char *zbuf,int zlen,int isdst) noex {
	int		rs = SR_FAULT ;
	int		znl = 0 ;
	if (zip && zbuf) {
	    cchar	*zp{} ;
	    if_constexpr (f_darwin) {
		custime		dt = time(nullptr) ;
		TM		tmo ;
	 	if ((rs = uc_localtime(&dt,&tmo)) >= 0) {
	            zip->zoff = (tmo.tm_gmtoff / 60) ;
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
/* end subroutine (getdefzinfo) */


