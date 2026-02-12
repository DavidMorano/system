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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |geustime(3u)| */
#include	<uclibmem.h>
#include	<uctimeconv.h>
#include	<bufsizevar.hh>
#include	<strn.h>		/* |strnwcpy(3uc)| */
#include	<strdcpy.h>
#include	<strwcpy.h>
#include	<sncpyx.h>
#include	<altzone.h>		/* <- special for bad systems */
#include	<localmisc.h>

#include	"tmtime.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int tmtime_zinit(tmtime *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_OK ;
	    if (op->znbuf == nullptr) ylikely {
	        if (char *a ; (rs = lm_zn(&a)) >= 0) ylikely {
		    op->znbuf = a ;
		    memclear(a,rs) ;
		    a[rs] = '\0' ;
	        } /* end if (memory-allocation) */
	    } /* end if (NULL zname) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tmtime_zinit) */

local int tmtime_zfini(tmtime *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->znbuf) {
		rs1 = lm_free(op->znbuf) ;
		if (rs >= 0) rs = rs1 ;
		op->znbuf = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tmtime_zfini) */

local int	tmtime_mktimer(tmtime *,int,time_t *) noex ;
local int	getznlen() noex ;


/* local variables */

static bufsizevar	znlen(bufsize_zn) ;

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */

cint tmtime::znlen  = 	getznlen() ;


/* exported subroutines */

int tmtime_ztime(tmtime *op,bool fz,time_t t) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (fz) {
	        rs = tmtime_timelocal(op,t) ;
	    } else {
	        rs = tmtime_timegm(op,t) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_ztime) */

int tmtime_timegm(tmtime *op,time_t t) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op)) >= 0) ylikely {
	    if (t == 0) t = getustime ;
	    if (TM tmd ; (rs = uc_timegm(&t,&tmd)) >= 0) ylikely {
	        if ((rs = tmtime_insert(op,&tmd)) >= 0) ylikely {
		    cchar *p = strwcpy(op->znbuf,"GMT",znlen) ;
	            op->gmtoff = 0 ; /* <- GMT */
	            rs = intconv(p - op->znbuf) ;
	        } /* end if (tmtime_insert) */
	    } /* end if */
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_zinit) */
	return rs ;
}
/* end subroutine (tmtime_timegm) */

int tmtime_timelocal(tmtime *op,time_t t) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op)) >= 0) ylikely {
	    if (t == 0) t = getustime ;
	    if (TM tmd ; (rs = uc_timelocal(&t,&tmd)) >= 0) ylikely {
	        rs = tmtime_insert(op,&tmd) ;
	    }
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_zinit) */
	return rs ;
}
/* end subroutine (tmtime_timelocal) */

/* tmtime <- tm */
int tmtime_insert(tmtime *op,CTM *tmp) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op,tmp)) >= 0) ylikely {
	    TM		tc = *tmp ;
	    op->gmtoff = -1 ;
	    op->sec	= tmp->tm_sec ;
	    op->min	= tmp->tm_min ;
	    op->hour	= tmp->tm_hour ;
	    op->mday	= tmp->tm_mday ;
	    op->mon	= tmp->tm_mon ;
	    op->year	= tmp->tm_year ;
	    op->wday	= tmp->tm_wday ;
	    op->yday	= tmp->tm_yday ;
	    op->isdst	= tmp->tm_isdst ;
	    if (tmp->tm_isdst < 0) {
	        time_t	t ; /* dummy */
	        rs = uc_mktime(&tc,&t) ;
	    } /* end if (need DST indicator) */
	    if (rs >= 0) ylikely {
	        cchar	*zp ;
	        if_constexpr (f_darwin) {
	            op->gmtoff = intconv(tc.tm_gmtoff) ;
	            zp = tc.tm_zone ;
	        } else {
	            cbool	f_isdst = (tc.tm_isdst > 0) ;
	            op->gmtoff = (f_isdst) ? altzone : timezone ;
	            zp = (f_isdst) ? tzname[1] : tzname[0] ;
	        } /* end if_constexpr (f_darwin) */
	        rs = intconv(strwcpy(op->znbuf,zp,znlen) - op->znbuf) ;
	    } /* end if (getting zone-name) */
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_zinit) */
	return rs ;
}
/* end subroutine (tmtime_insert) */

/* tmtime -> tm */
int tmtime_extract(tmtime *op,TM *tmp) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op,tmp)) >= 0) ylikely {
	    tmp->tm_sec		= op->sec ;
	    tmp->tm_min		= op->min ;
	    tmp->tm_hour	= op->hour ;
	    tmp->tm_mday	= op->mday ;
	    tmp->tm_mon		= op->mon ;
	    tmp->tm_year	= op->year ;
	    tmp->tm_wday	= op->wday ;
	    tmp->tm_yday	= op->yday ;
	    tmp->tm_isdst	= op->isdst ;
	    if_constexpr (f_darwin) {
	        tmp->tm_gmtoff = long(op->gmtoff) ;
	        tmp->tm_zone = op->znbuf ;
	    } /* end if_constexpr (f_darwin) */
	    if (rs < 0) {
		op->dtor() ;
	    }
	} /* end if (tmtime_zinit) */
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

int tmtime_getzn(tmtime *op,char *rbuf,int rlen) noex {
    	int		rs ;
	if (op && rbuf) {
    	    rs = SR_NOTOPEN ;
	    if (op->znbuf) {
	        rs = sncpy(rbuf,rlen,op->znbuf) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tmtime_getzn) */

int tmtime_loadzn(tmtime *op,cchar *sp,int sl) noex {
    	int		rs ;
	if ((rs = tmtime_zinit(op,sp)) >= 0) ylikely {
	    cchar *p = strdcpy1w(op->znbuf,op->znlen,sp,sl) ;
	    rs = intconv(p - op->znbuf) ;
	} /* end if (tmtime_zinit) */
	return rs ;
}
/* end subroutine (tmtime_loadzn) */


/* local subroutines */

local int tmtime_mktimer(tmtime *op,int fadj,time_t *tp) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op)) >= 0) ylikely {
	    time_t	t = 0 ;
	    if (TM tmd ; (tmtime_extract(op,&tmd)) >= 0) ylikely {
	        if ((rs = uc_mktime(&tmd,&t)) >= 0) ylikely {
	            cint	taroff = op->gmtoff ;
	            int 	locoff ;
	            cbool	f_isdst = (tmd.tm_isdst > 0) ;
	            locoff = intconv((f_isdst) ? altzone : timezone) ;
	            t += (taroff - locoff) ;
	            if (fadj) ylikely {
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
	} /* end if (tmtime_zinit) */
	return rs ;
}
/* end subroutine (tmtime_mktimer) */

int tmtime::timex(bool fz,time_t t) noex {
	return tmtime_timex(this,fz,t) ;
}

int tmtime::timegm(time_t t) noex {
	return tmtime_timegm(this,t) ;
}

int tmtime::timelocal(time_t t) noex {
	return tmtime_timelocal(this,t) ;
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

int tmtime::getzn(char *rbuf,int rlen) noex {
	return tmtime_getzn(this,rbuf,rlen) ;
}

int tmtime::loadzn(cchar *sp,int sl) noex {
	return tmtime_loadzn(this,sp,sl) ;
}

void tmtime::dtor() noex {
	if (cint rs = tmtime_zfini(this) ; rs < 0) {
	    ulogerror("tmtime",rs,"dtor-free") ;
	}
} /* end method (tmtime::dtor) */

local int getznlen() noex {
    	int		rs ;
	if ((rs = znlen) < 0) {
	    ulogerror("tmtime",rs,"getznlen") ;
	} /* end if (znlen) */
	return rs ;
} /* end subroutine (getznlen) */


