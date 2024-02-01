/* getdefzinfo SUPPORT */
/* lang=C++20 */

/* this is supposed to provide an OS-independent time management operation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We return some default time-zone information.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>
#include	<ctime>			/* <- for |time(2)| */
#include	<usystem.h>
#include	<usysflag.h>
#include	<strwcpy.h>
#include	<altzone.h>
#include	<localmisc.h>

#include	"getdefzinfo.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int getdefzinfo(GETDEFZINFO *zip,int isdst) noex {
	int		rs = SR_FAULT ;
	if (zip) {
	    cchar	*zp ;
	    if constexpr (f_darwin) {
		const time_t	dt = time(nullptr) ;
		TM		tmo ;
	 	if ((rs = uc_localtime(&dt,&tmo)) >= 0) {
	            zip->zoff = (tmo.tm_gmtoff / 60) ;
	            zp = tmo.tm_zone ;
		}
	    } else {
	        int	f_daylight ;
		rs = SR_OK ;
	        tzset() ;
	        f_daylight = (isdst >= 0) ? isdst : daylight ;
	        zip->zoff = (((f_daylight) ? altzone : timezone) / 60) ;
	        zp = (f_daylight) ? tzname[1] : tzname[0] ;
	    } /* end if-constexpr (f_darwin) */
	    if (rs >= 0) {
		cint	znamelen = GETDEFZINFO_ZNAMELEN ;
	        rs = strwcpy(zip->zname,zp,znamelen) - zip->zname ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getdefzinfo) */


