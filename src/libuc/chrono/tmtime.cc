/* tmtime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide OS-independent time management operations */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_USEMORE	1		/* use more field in structure TM */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

	= 2026-04-28, David A­D­ Morano
	I modified (this POS) to remove references to |altzone|.

*/

/* Copyright © 2008,2026 David A­D­ Morano.  All rights reserved. */

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

	Finally, the Darwin OS (from Apple) has some extra fields
	in its |TM| object that needs handling.

	Notes:
	1. The |zos_get(3uc)| subroutine below returns MINUTES west
	of GMT.  If the zone-offset is indicated to be eastwards
	of GMT, the value returned is negative.  In all cases, the
	value returned is in MINUTES.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |geustime(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<uctimeconv.h>		/* LIBUC */
#include	<ucstrftime.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<strn.h>		/* LIBUC |strnwcpy(3uc)| */
#include	<strdcpy.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<zos.h>			/* LIBUC (part of |chrono|) */
#include	<localmisc.h>		/* LIBU |DECBUFLEN| */
#include	<dprint.hh>		/* LIBU |DPRINT(3u)| */

#include	"tmtime.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif
#ifndef	CF_USEMORE
#define	CF_USEMORE	1		/* use more field in structure TM */
#endif


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
	        } /* end if (memory-acquire) */
	    } /* end if (NULL zname) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tmtime_zinit) */

local int tmtime_zfini(tmtime *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->znbuf) {
		rs1 = lm_free(op->znbuf) ;
		if (rs >= 0) rs = rs1 ;
		op->znbuf = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tmtime_zfini) */

local int	tmtime_mktimer	(tmtime *,int,time_t *) noex ;
local int	tmtime_moreuse	(tmtime *,con TM *) noex ;
local int	tmtime_morecalc	(tmtime *,con TM *) noex ;
local int	getznlen() noex ;


/* local variables */

static bufsizevar	znlen		(bufsize_zn) ;
constexpr int		OneMinute	= 60 ;
constexpr int		OneHour		= (60 * 60) ;
constexpr int		MinusOne	= -1 ;
constexpr bool		f_debug		= CF_DEBUG ;
constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_linux		= F_LINUX ;


/* exported variables */

cint	tmtime::znlen		= getznlen() ;
cint	tmtime::baseyear	= TMTIME_YEARBASE ;
cint	baseyear		= TMTIME_YEARBASE ;
cbool	f_usemore		= CF_USEMORE ;


/* exported subroutines */

/* 0=GMT 1=local */
int tmtime_timex(tmtime *op,bool fz,time_t t) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (fz) {
	        rs = tmtime_timelocal(op,t) ;
	    } else {
	        rs = tmtime_timegm(op,t) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tmtime_timex) */

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
	    } /* end if (error) */
	} /* end if (tmtime_zinit) */
	return rs ;
} /* end subroutine (tmtime_timegm) */

int tmtime_timelocal(tmtime *op,time_t t) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op)) >= 0) ylikely {
	    if (t == 0) t = getustime ;
	    if (TM tmd ; (rs = uc_timelocal(&t,&tmd)) >= 0) ylikely {
	        rs = tmtime_insert(op,&tmd) ;
	    }
	    if (rs < 0) {
		op->dtor() ;
	    } /* end if (error) */
	} /* end if (tmtime_zinit) */
	return rs ;
} /* end subroutine (tmtime_timelocal) */

/* tmtime <- tm */
int tmtime_insert(tmtime *op,CTM *tmp) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op,tmp)) >= 0) ylikely {
	    TM		tc = *tmp ;
	    op->gmtoff	= -1 ;
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
	        if_constexpr (f_usemore && (f_darwin || f_linux)) {
		    rs = tmtime_moreuse(op,tmp) ;
	        } else {
		    rs = tmtime_morecalc(op,tmp) ;
	        } /* end if_constexpr (f_darwin) */
	    } /* end if (getting zone-name) */
	    if (rs < 0) {
		op->dtor() ;
	    } /* end if (error) */
	} /* end if (tmtime_zinit) */
	return rs ;
} /* end subroutine (tmtime_insert) */

/* tmtime -> tm */
int tmtime_extract(tmtime *op,mut TM *tmp) noex {
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
	    if_constexpr (f_darwin || f_linux) {
	        tmp->tm_gmtoff = long(op->gmtoff) ;
	        tmp->tm_zone = op->znbuf ;
	    } else {
	        tmp->tm_gmtoff = long(op->gmtoff) ;
	        tmp->tm_zone = op->znbuf ;
	    } /* end if_constexpr (f_darwin) */
	    if (rs < 0) {
		op->dtor() ;
	    } /* end if (error) */
	} /* end if (tmtime_zinit) */
	return rs ;
} /* end subroutine (tmtime_extract) */

int tmtime_mktime(tmtime *op,time_t *tp) noex {
	return tmtime_mktimer(op,0,tp) ;
} /* end subroutine (tmtime_mktime) */

int tmtime_adjtime(tmtime *op,time_t *tp) noex {
	return tmtime_mktimer(op,1,tp) ;
} /* end subroutine (tmtime_adjtime) */

int tmtime_getzn(tmtime *op,char *rbuf,int rlen) noex {
    	int		rs ;
	if (op && rbuf) ylikely {
    	    rs = SR_NOTOPEN ;
	    if (op->znbuf) ylikely {
	        rs = sncpy(rbuf,rlen,op->znbuf) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tmtime_getzn) */

int tmtime_loadzn(tmtime *op,cchar *sp,int sl) noex {
    	int		rs ;
	if ((rs = tmtime_zinit(op,sp)) >= 0) ylikely {
	    cchar *p = strdcpy1w(op->znbuf,op->znlen,sp,sl) ;
	    rs = intconv(p - op->znbuf) ;
	} /* end if (tmtime_zinit) */
	return rs ;
} /* end subroutine (tmtime_loadzn) */


/* local subroutines */

local int tmtime_mktimer(tmtime *op,int fadj,time_t *tp) noex {
	int		rs ;
	if ((rs = tmtime_zinit(op)) >= 0) ylikely {
	    time_t	t = 0 ;
	    if (TM tmd ; (rs = tmtime_extract(op,&tmd)) >= 0) ylikely {
	        if ((rs = uc_mktime(&tmd,&t)) >= 0) ylikely {
	            cint	taroff = op->gmtoff ;
	            cint 	locoff = intconv(tmd.tm_gmtoff) ;
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
	            } /* end if (fadj) */
	        } /* end if (uc_mktime) */
	    } /* end if (ttime_extract) */
	    if (tp) {
	        *tp = (rs >= 0) ? t : 0 ;
	    }
	} /* end if (tmtime_zinit) */
	return rs ;
} /* end subroutine (tmtime_mktimer) */

#if	CF_USEMORE && (F_DARWIN || F_LINUX)
local int tmtime_moreuse(tmtime *op,con TM *tmp) noex {
    	int		rs ;
	ccharp		zp = tmp->tm_zone ;
	{
	   op->gmtoff = intconv(tmp->tm_gmtoff) * MinusOne ;
	   rs = intconv(strwcpy(op->znbuf,zp,op->znlen) - op->znbuf) ;
	}
	return rs ;
} /* end subroutine (tmtime_moreuse) */
#else /* CF_USEMORE && (F_DARWIN || F_LINUX) */
local int tmtime_moreuse(tmtime *,con TM *) noex {
	return SR_OK ;
} /* end subroutine (tmtime_moreuse) */
#endif /* CF_USEMORE && (F_DARWIN || F_LINUX) */

local int tmtime_morecalc(tmtime *op,con TM *tmp) noex {
    	int		rs ;
	int		zl = 0 ; /* return-value */
	cchar		*fmt = "%Z" ;
	DPRINTF("ent\n") ;
	if ((rs = uc_strftime(op->znbuf,op->znlen,fmt,tmp)) >= 0) ylikely {
	    cint	dlen = DECBUFLEN ;
	    char	dbuf[DECBUFLEN + 1] ;
	    zl = rs ;
	    fmt = "%z" ;
	    if ((rs = uc_strftime(dbuf,dlen,fmt,tmp)) >= 0)  ylikely {
		if (int v ; (rs = zos_get(dbuf,dlen,&v)) >= 0) ylikely {
	            op->gmtoff = v * OneMinute ; /* calculate seconds */
		} /* end if (zos_get) */
	    } /* end if (uc_strftime) */
	} /* end if (uc_strftime) */
	DPRINTF("ret rs=%d zl=%d\n",rs,zl) ;
    	return (rs >= 0) ? zl : rs ;
} /* end subroutine (tmtime_morecalc) */

int tmtime::timex(bool fz,time_t t) noex {
	return tmtime_timex(this,fz,t) ;
} /* end method */

int tmtime::timegm(time_t t) noex {
	return tmtime_timegm(this,t) ;
} /* end method */

int tmtime::timelocal(time_t t) noex {
	return tmtime_timelocal(this,t) ;
} /* end method */

int tmtime::insert(CTM *tmp) noex {
	return tmtime_insert(this,tmp) ;
} /* end method */

int tmtime::extract(TM *tmp) noex {
	return tmtime_extract(this,tmp) ;
} /* end method */

int tmtime::mktime(time_t *tp) noex {
	return tmtime_mktime(this,tp) ;
} /* end method */

int tmtime::adjtime(time_t *tp) noex {
	return tmtime_adjtime(this,tp) ;
} /* end method */

int tmtime::getzn(char *rbuf,int rlen) noex {
	return tmtime_getzn(this,rbuf,rlen) ;
} /* end method */

int tmtime::loadzn(cchar *sp,int sl) noex {
	return tmtime_loadzn(this,sp,sl) ;
} /* end method */

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


