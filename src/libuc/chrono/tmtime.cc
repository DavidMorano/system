/* tmtime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide OS-independent time management operations */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tmtime

	Description:
	This code provides an interface to some time mangement
	functions that hides some of the (stupid) gory details of
	the internal UNIX® time-zone managemtnt system.

	Implementation notes:

	Calling |ftime(3c)| sets the local time-zone information
	(through) a secret call to (as you know) |tzset(3c)|.

	Note also that the Darwin OS (used on Macs as the core of
	MacOS) does not maintain the normal external variables that
	are set by |tzset(3c)| as previous, more traditional, OSes
	did.  This is a positive development and one that should
	have been in there from the beginning, but provision has
	to made for it none-the-less.

	Finally, note that SlowLaris has a 'define' bug in that it
	does not declare the 'altzone' variable unless some other
	defines are made (see the code).  It is not clear if and
	when this will be or has been fixed.  This subroutine does
	not currently use the 'altzone' variable anyway.

	Finally, the Darwin OS (from Apple) has some extra fields
	in its |TM| object that needs handling.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<strn.h>		/* |strnwcpy(3uc)| */
#include	<strwcpy.h>
#include	<altzone.h>		/* <- special for bad systems */
#include	<localmisc.h>

#include	"tmtime.hh"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int tmtime_ctor(tmtime *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    if (op->zname == nullptr) {
	        if (char *a ; (rs = malloc_zn(&a)) >= 0) {
		    op->zname = a ;
		    memclear(a,rs) ;
		    a[rs] = '\0' ;
	        } /* end if (memory-allocation) */
	    } /* end if (NULL zname) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_ctor) */

static int	tmtime_mktimer(tmtime *,int,time_t *) noex ;


/* local variables */

static bufsizevar	znlen(getbufsize_zn) ;

constexpr bool		f_darwin = F_DARWIN ;


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
	int		rs ;
	if ((rs = tmtime_ctor(op)) >= 0) {
	    if (t == 0) t = time(nullptr) ;
	    if (TM tmd ; (rs = uc_gmtime(&t,&tmd)) >= 0) {
	        if ((rs = tmtime_insert(op,&tmd)) >= 0) {
	            op->gmtoff = 0 ;
	            rs = intconv(strwcpy(op->zname,"GMT",znlen) - op->zname) ;
	        }
	    } /* end if */
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_ctor) */
	return rs ;
}
/* end subroutine (tmtime_gmtime) */

int tmtime_localtime(tmtime *op,time_t t) noex {
	int		rs ;
	if ((rs = tmtime_ctor(op)) >= 0) {
	    if (t == 0) t = time(nullptr) ;
	    if (TM tmd ; (rs = uc_localtime(&t,&tmd)) >= 0) {
	        rs = tmtime_insert(op,&tmd) ;
	    }
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_ctor) */
	return rs ;
}
/* end subroutine (tmtime_localtime) */

int tmtime_insert(tmtime *op,CTM *tmp) noex {
	int		rs ;
	if ((rs = tmtime_ctor(op,tmp)) >= 0) {
	    TM		tc = *tmp ;
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
	        cchar	*zp ;
	        if_constexpr (f_darwin) {
	            op->gmtoff = intconv(tc.tm_gmtoff) ;
	            zp = tc.tm_zone ;
	        } else {
	            cbool	f_isdst = (tc.tm_isdst > 0) ;
	            op->gmtoff = (f_isdst) ? altzone : timezone ;
	            zp = (f_isdst) ? tzname[1] : tzname[0] ;
	        } /* end if_constexpr (f_darwin) */
	        rs = intconv(strwcpy(op->zname,zp,znlen) - op->zname) ;
	    } /* end if (getting zone-name) */
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_ctor) */
	return rs ;
}
/* end subroutine (tmtime_insert) */

int tmtime_extract(tmtime *op,TM *tmp) noex {
	int		rs ;
	if ((rs = tmtime_ctor(op,tmp)) >= 0) {
	    tmp->tm_sec = op->sec ;
	    tmp->tm_min = op->min ;
	    tmp->tm_hour = op->hour ;
	    tmp->tm_mday = op->mday ;
	    tmp->tm_mon = op->mon ;
	    tmp->tm_year = op->year ;
	    tmp->tm_wday = op->wday ;
	    tmp->tm_yday = op->yday ;
	    tmp->tm_isdst = op->isdst ;
	    if_constexpr (f_darwin) {
	        tmp->tm_gmtoff = op->gmtoff ;
	        tmp->tm_zone = op->zname ;
	    } /* end if_constexpr (f_darwin) */
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_ctor) */
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

static int tmtime_mktimer(tmtime *op,int fadj,time_t *tp) noex {
	int		rs ;
	if ((rs = tmtime_ctor(op)) >= 0) {
	    time_t	t = 0 ;
	    if (TM tmd ; (tmtime_extract(op,&tmd)) >= 0) {
	        if ((rs = uc_mktime(&tmd,&t)) >= 0) {
	            cint	taroff = op->gmtoff ;
	            int 	locoff ;
	            cbool	f_isdst = (tmd.tm_isdst > 0) ;
	            locoff = intconv((f_isdst) ? altzone : timezone) ;
	            t += (taroff - locoff) ;
	            if (fadj) {
	                op->sec = tmd.tm_sec ;
	                op->min = tmd.tm_min ;
	                op->hour = tmd.tm_hour ;
	                op->mday = tmd.tm_mday ;
	                op->mon = tmd.tm_mon ;
	                op->year = tmd.tm_year ;
	                op->wday = tmd.tm_wday ;
	                op->yday = tmd.tm_yday ;
	                op->isdst = tmd.tm_isdst ;
	            }
	        } /* end if (uc_mktime) */
	    } /* end if (ttime_extract) */
	    if (tp) {
	        *tp = (rs >= 0) ? t : 0 ;
	    }
	} /* end if (tmtime_ctor) */
	return rs ;
}
/* end subroutine (tmtime_mktimer) */

int tmtime::ztime(bool fz,time_t t) noex {
	return tmtime_ztime(this,fz,t) ;
}

int tmtime::gmtime(time_t t) noex {
	return tmtime_gmtime(this,t) ;
}

int tmtime::localtime(time_t t) noex {
	return tmtime_localtime(this,t) ;
}

int tmtime::insert(CTM *tmp) noex {
	return tmtime_insert(this,tmp) ;
}

int tmtime::extract(TM *tmp) noex {
	return tmtime_extract(this,tmp) ;
}

int tmtime::mktime(time_t *tp) noex {
	return tmtime_mktime(this,tp) ;
}

int tmtime::adjtime(time_t *tp) noex {
	return tmtime_adjtime(this,tp) ;
}

void tmtime::dtor() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
    	if (zname) {
	    rs1 = uc_free(zname) ;
	    if (rs >= 0) rs = rs1 ;
	    zname = nullptr ;
	}
	if (rs < 0) {
	    ulogerror("tmtime",rs,"dtor-free") ;
	}
}
/* end method (tmtime::dtor) */


