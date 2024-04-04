/* tmtime SUPPORT */
/* lang=C++20 */

/* this is supposed to provide OS-independent time management operations */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This code provides an interface to some time mangement
	functions that hides some of the (stupid) gory details of
	the internal UNIX® time-zone managemtnt system.

	Implementation notes:

	Calling |ftime(3c)| sets the local time-zone information
	(through) a secret call to (as you know) |tzset(3c)|.

	Note also that the Darwin OS (used on Macs as the core of
	MacOS) does not maintain the normal external variables that
	are set by |tzset(3c)| as previous, more traditional, OSes
	did. This is a positive development and one that should
	have been in there from the beginning, but provision has
	to made for it none-the-less.

	Finally, note that SlowLaris has a 'define' bug in that it
	does not declare the 'altzone' variable unless some other
	defines are made (see the code). It is not clear if and
	when this will be or has been fixed.  This subroutine does
	not currently use the 'altzone' variable anyway.

	Finally, the Darwin OS (from Apple) has some extra fields
	in its |TM| object that needs handling.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<ctime>
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<strwcpy.h>
#include	<altzone.h>		/* <- special for bad systems */
#include	<localmisc.h>

#include	"tmtime.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		tmtime_gmtime(tmtime *,time_t) noex ;
    int		tmtime_gmtime(tmtime *,time_t) noex ;
    int		tmtime_insert(tmtime *,TM *) noex ;
    int		tmtime_extract(tmtime *,TM *) noex ;
}


/* local structures */


/* forward references */

static int	tmtime_mktimer(tmtime *,int,time_t *) noex ;


/* local variables */

constexpr int	znamelen = TMTIME_ZNAMELEN ;
constexpr bool	f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int tmtime_ztime(tmtime *op,bool fz,time_t t) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (fz) {
	        rs = tmtime_localtime(op,t) ;
	    } else {
	        rs = tmtime_gmtime(op,t) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_ztime) */

int tmtime_gmtime(tmtime *op,time_t t) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    TM		tms ;
	    if (t == 0) t = time(nullptr) ;
	    memclear(op) ;
	    if ((rs = uc_gmtime(&t,&tms)) >= 0) {
	        if ((rs = tmtime_insert(op,&tms)) >= 0) {
	            op->gmtoff = 0 ;
	            rs = strwcpy(op->zname,"GMT",znamelen) - op->zname ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_gmtime) */

int tmtime_localtime(tmtime *op,time_t t) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    TM	tms ;
	    if (t == 0) t = time(nullptr) ;
	    memclear(op) ;
	    if ((rs = uc_localtime(&t,&tms)) >= 0) {
	        rs = tmtime_insert(op,&tms) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_localtime) */

int tmtime_insert(tmtime *op,TM *tmp) noex {
	int		rs = SR_FAULT ;
	if (op && tmp) {
	    TM		tc = *tmp ;
	    cchar	*zp ;
	    rs = memclear(op) ;
	    op->gmtoff = 0 ;
	    op->sec = tmp->tm_sec ;
	    op->min = tmp->tm_min ;
	    op->hour = tmp->tm_hour ;
	    op->mday = tmp->tm_mday ;
	    op->mon = tmp->tm_mon ;
	    op->year = tmp->tm_year ;
	    op->wday = tmp->tm_wday ;
	    op->yday = tmp->tm_yday ;
	    op->isdst = tmp->tm_isdst ;
	    if (tmp->tm_isdst < 0) {
	        time_t	t ; /* dummy */
	        rs = uc_mktime(&tc,&t) ;
	    } /* end if (need DST indicator) */
	    if (rs >= 0) {
	        if constexpr (f_darwin) {
	            op->gmtoff = tc.tm_gmtoff ;
	            zp = tc.tm_zone ;
	        } else {
	            bool	f_isdst = (tc.tm_isdst > 0) ;
	            op->gmtoff = (f_isdst) ? altzone : timezone ;
	            zp = (f_isdst) ? tzname[1] : tzname[0] ;
	        } /* end if-constexpr (f_darwin) */
	        rs = strwcpy(op->zname,zp,znamelen) - op->zname ;
	    } /* end if (getting zone-name) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_insert) */

int tmtime_extract(tmtime *op,TM *tmp) noex {
	int		rs = SR_FAULT ;
	if (op && tmp) {
	    rs = SR_OK ;
	    memclear(tmp) ;
	    tmp->tm_sec = op->sec ;
	    tmp->tm_min = op->min ;
	    tmp->tm_hour = op->hour ;
	    tmp->tm_mday = op->mday ;
	    tmp->tm_mon = op->mon ;
	    tmp->tm_year = op->year ;
	    tmp->tm_wday = op->wday ;
	    tmp->tm_yday = op->yday ;
	    tmp->tm_isdst = op->isdst ;
	    if constexpr (f_darwin) {
	        tmp->tm_gmtoff = op->gmtoff ;
	        tmp->tm_zone = op->zname ;
	    } /* end if-constexpr (f_darwin) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_extract) */

int tmtime_mktime(tmtime *op,time_t *tp) noex {
	return tmtime_mktimer(op,0,tp) ;
}
/* end subroutine (tmtime_mktime) */

int tmtime_adjtime(tmtime *op,time_t *tp) noex {
	return tmtime_mktimer(op,1,tp) ;
}
/* end subroutine (tmtime_adjtime) */


/* local subroutines */

int tmtime_mktimer(tmtime *op,int f_adj,time_t *tp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    TM		tms ;
	    time_t	t = 0 ;
	    tmtime_extract(op,&tms) ;
	    if ((rs = uc_mktime(&tms,&t)) >= 0) {
	        int	taroff = op->gmtoff ;
	        int 	locoff ;
	        cbool	f_isdst = (tms.tm_isdst > 0) ;
	        locoff = (f_isdst) ? altzone : timezone ;
	        t += (taroff - locoff) ;
	        if (f_adj) {
	            op->sec = tms.tm_sec ;
	            op->min = tms.tm_min ;
	            op->hour = tms.tm_hour ;
	            op->mday = tms.tm_mday ;
	            op->mon = tms.tm_mon ;
	            op->year = tms.tm_year ;
	            op->wday = tms.tm_wday ;
	            op->yday = tms.tm_yday ;
	            op->isdst = tms.tm_isdst ;
	        }
	    } /* end if (uc_mktime) */
	    if (tp) {
	        *tp = (rs >= 0) ? t : 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_mktimer) */


