/* timest_nist SUPPORT */
/* lang=C++20 */

/* convert UNIX® time into a Julian like character string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little subroutine formats a time string that is close
	to what NIST itself uses for the 'daytime' Internet service.

	We are currently not exact with this string format.  The
	NIST format (that has its own RFC) contains information
	that we do not easily have available to us.  Everything
	except for the time itself is not really available to us
	at the present (we being too lazy to really keep it all and
	track it all), but we do try to do something with
	dalight-savings-time in the string format.  We put out the
	50' code for daylight-savings time and '00' for standard
	time.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<usupport.h>		/* |ulogerror(3u)| */
#include	<estrings.h>
#include	<snx.h>
#include	<getmjd.h>
#include	<nistinfo.h>
#include	<localmisc.h>

#include	"timestr.h"


/* local defines */

#ifndef	ORG
#define	ORG		"RNS"
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

char *timestr_nist(time_t t,nistinfo *nip,char *tbuf) noex {
	cint		tlen = NISTINFO_BUFLEN ;
	int		rs = SR_FAULT ;
	if (nip && tbuf) {
	    tbuf[0] = '\0' ;
	    rs = SR_DOM ;
	    if (t >= 0) {
	        TM	tsz, *tszp = &tsz ;
	        TM	tsl, *tslp = &tsl ;
	        if (t == 0) t = time(nullptr) ;
	        if ((rs = uc_gmtime(&t,tszp)) >= 0) {
	            if ((rs = uc_localtime(&t,tslp)) >= 0) {
	                cint	y = tszp->tm_year ;
	                cint	m = tszp->tm_mon ;
	                cint	d = tszp->tm_mday ;
	                if ((rs = getmjd(y,m,d)) >= 0) {
	                    cint	mjd = rs ;
	                    cint	adv_int = (nip->adv / 10) ;
	                    cint	adv_fra = (nip->adv % 10) ;
		            cint	olen = NISTINFO_ORGLEN ;
	                    int		tt = nip->tt ;
	                    cchar	*deforg = ORG ;
	                    cchar	*fmt ;
	                    if (tt == 0) {
	                        if (tslp->tm_isdst) tt = 50 ;
	                    }
	                    if (nip->org[0] != '\0') {
			        cint	ol = strlen(nip->org) ;
			        if (ol > olen) {
			            nip->org[olen] = '\0' ;
			        }
		            } else {
			        strdcpy1(nip->org,olen,deforg) ;
	                    }
	                    fmt = "%05u %02u-%02u-%02u %02u:%02u:%02u"
	                        " %02u %u %1u %03u.%01u UTC(%s) *" ;
	                    rs = snwprintf(tbuf,tlen,fmt,
	                        mjd,
	                        (tszp->tm_year % NYEARS_CENTURY),
	                        (tszp->tm_mon + 1),
	                        tszp->tm_mday,
	                        tszp->tm_hour,
	                        tszp->tm_min,
	                        tszp->tm_sec,
	                        tt,
	                        nip->l,
	                        nip->h,
	                        adv_int,adv_fra,
	                        nip->org) ;
	                } /* end if (getmjd) */
	            } /* end if (uc_getlocaltime) */
	        } /* end if (uc_gmtime) */
	    } /* end if (valid) */
	    if (rs < 0) tbuf[0] = '\0' ;
	} /* end if (non-null) */
	if (rs < 0) {
	    ulogerror("timestr",rs,"nist") ;
	}
	return (rs >= 0) ? tbuf : nullptr ;
}
/* end subroutine (timest_nist) */


