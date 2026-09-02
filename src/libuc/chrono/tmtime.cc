/* tmtime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide OS-independent time management operations */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_USEMORE	1		/* use more field in structure TM */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written.

	- 1999-10-01, David A­D­ Morano
	I modified (this POS) to remove references to |altzone|.
	The |altzone| external variable is simply not available on
	some non-SysV systems.  OK, will disclose.  In an attempt
	to port this code to the MacOS (Apple-Darwin) operating
	system, I was confronted with the fact that MacOS is not
	(at all) like the former Sys-V type systems (that is: no
	|altzone| global variable).  It could be argued that code
	like this should never have referred to any global variables
	at all, but Sys-V was messed up in this and many other ways
	already.  I argue that the MacOS version of the TM(3c)
	object structure is the proper way to represent these sorts
	of date-time features.  I modified this code to try to be
	portable for both Sys-V like systems (those w/ those global
	variable and without the additional MacOS structure object
	variables) and for non-Sys-V operating systems that follow
	the MacOS implementation.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

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
	2. This code is complicated enough (even for me), so there
	could be bugs in this code that I am not (yet) aware of.
	Enjoy.

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
#include	<ucsysmisc.h>		/* LIBUC */
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
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_USEMORE
#define	CF_USEMORE	1		/* use more fields in structure TM */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum adjustments {
    	adjustment_no,
	adjustment_yes,
	adjustment_overlast
} ; /* end enum (adjustments) */

namespace {
    struct offer {
	int		offval ; /* ± minutes west of GMT */
	bool		finit ;
	int operator () (intp = nullptr) noex ;
    } ; /* end struct (offer) */
} /* end namespace */


/* forward references */

local int initer() noex ;

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
		    rs = initer() ;
		    if (rs < 0) {
			lm_free(a) ;
			op->znbuf = nullptr ;
		    } /* end if (error) */
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

local void	tm_loadmore	(TM *,con TMTIME *) noex ;
local int	tm_getoff	(con TM *,intp) noex ;

local int	tmtime_mktimer	(tmtime *,adjustments,mut time_t *) noex ;
local int	tmtime_moreuse	(tmtime *,con TM *) noex ;
local int	tmtime_morecalc	(tmtime *,con TM *) noex ;
local int	getznlen	() noex ;


/* local variables */

static bufsizevar	znlen		(bufsize_zn) ;
static offer		gmoff ;
cint			baseyear	= TMTIME_YEARBASE ;
cint			OneMinute	= 60 ;
cint			OneHour		= (60 * 60) ;
cint			MinusOne	= -1 ;
cbool			f_debug		= CF_DEBUG ;
cbool			f_usemore	= CF_USEMORE ;
cbool			f_darwin	= F_DARWIN ;
cbool			f_linux		= F_LINUX ;


/* exported variables */

int	tmtime::znlen		= 0 ;	/* initialized for real on first use */
int	tmtime::baseyear	= TMTIME_YEARBASE ;


/* exported subroutines */

/* 0=GMT 1=local */
int tmtime_timex(tmtime *op,time_t t,bool flocal) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (flocal) {
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
	    } /* end if (TM) */
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
	    } /* end if (TM) */
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
	    TM		tc = *tmp ; /* copy */
	    op->gmtoff	= 0 ;
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
		tm_loadmore(tmp,op) ;
	    } /* end if_constexpr (f_darwin || f_linux) */
	    if (rs < 0) {
		op->dtor() ;
	    } /* end if (error) */
	} /* end if (tmtime_zinit) */
	return rs ;
} /* end subroutine (tmtime_extract) */

int tmtime_mktime(tmtime *op,time_t *tp) noex {
	return tmtime_mktimer(op,adjustment_no,tp) ;
} /* end subroutine (tmtime_mktime) */

int tmtime_adjtime(tmtime *op,time_t *tp) noex {
	return tmtime_mktimer(op,adjustment_yes,tp) ;
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

local int tmtime_mktimer(tmtime *op,adjustments fadj,mut time_t *timep) noex {
	int		rs ;
	DPRINTF("ent\n") ;
	if ((rs = tmtime_zinit(op)) >= 0) ylikely {
	    time_t	t = 0 ;
	    DPRINTF("tmtime gmtoff=%d\n",op->gmtoff) ;
	    if (TM tmd ; (rs = tmtime_extract(op,&tmd)) >= 0) ylikely {
	        DPRINTF("tm gmtoff=%ld\n",tmd.tm_gmtoff) ;
	        if ((rs = uc_mktime(&tmd,&t)) >= 0) ylikely {
	            cint	taroff = op->gmtoff ;
	            DPRINTF("tm gmtoff=%ld\n",tmd.tm_gmtoff) ;
	            if (int locoff ; (rs = tm_getoff(&tmd,&locoff)) >= 0) {
	                cint tdiff = (taroff - (neg locoff)) ;
	                DPRINTF("tm taroff=%d\n",taroff) ;
	                DPRINTF("tm locoff=%d\n",locoff) ;
	                DPRINTF("time-diff=%d\n",tdiff) ;
	                t += tdiff ;	/* -> add difference (secs w-of-GMT) */
	                if (fadj) ylikely {
	                    op->sec	= tmd.tm_sec ;
	                    op->min	= tmd.tm_min ;
	                    op->hour	= tmd.tm_hour ;
	                    op->mday	= tmd.tm_mday ;
	                    op->mon	= tmd.tm_mon ;
	                    op->year	= tmd.tm_year ;
	                    op->wday	= tmd.tm_wday ;
	                    op->yday	= tmd.tm_yday ;
	                    op->isdst	= tmd.tm_isdst ;
	                } /* end if (fadj) */
		    } /* end if (tm_getoff) */
	        } /* end if (uc_mktime) */
	    } /* end if (TM) */
	    if (timep) {
	        *timep = (rs >= 0) ? t : 0 ;
	    }
	} /* end if (tmtime_zinit) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (tmtime_mktimer) */

#if	CF_USEMORE && (F_DARWIN || F_LINUX)
local int tmtime_moreuse(tmtime *op,con TM *tmp) noex {
    	int		rs ;
	ccharp		zp = tmp->tm_zone ;
	{
	   op->gmtoff = intconv(tmp->tm_gmtoff) * MinusOne ;
	   rs = intconv(strwcpy(op->znbuf,zp,op->znlen) - op->znbuf) ;
	} /* end */
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
	if ((rs = uc_strftime(op->znbuf,op->znlen,fmt,tmp)) > 0) ylikely {
	    cint	dlen = DECBUFLEN ;
	    char	dbuf[DECBUFLEN + 1] ;
	    zl = rs ;
	    fmt = "%z" ;
	    if ((rs = uc_strftime(dbuf,dlen,fmt,tmp)) > 0)  ylikely {
		if (int v ; (rs = zos_get(dbuf,rs,&v)) >= 0) ylikely {
	            op->gmtoff = v * OneMinute ; /* calculate seconds */
		} /* end if (zos_get) */
	    } /* end if (uc_strftime) */
	} /* end if (uc_strftime) */
	DPRINTF("ret rs=%d zl=%d\n",rs,zl) ;
    	return (rs >= 0) ? zl : rs ;
} /* end subroutine (tmtime_morecalc) */

int tmtime::timex(time_t t,bool flocal) noex {
	return tmtime_timex(this,t,flocal) ;
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

#if	CF_USEMORE && (F_DARWIN || F_LINUX)
local void tm_loadmore(TM *tmp,con TMTIME *op) noex {
	tmp->tm_gmtoff = long(neg op->gmtoff) ;
	tmp->tm_zone = op->znbuf ;
} /* end subroutine (tm_loadmore) */
#else /* CF_USEMODE */
local void tm_loadmore(TM *tmp,con TMTIME *op) noex {
    	(void) tmp ;
	(void) op ;
} /* end subroutine (tm_loadmore) */
#endif /* CF_USEMORE && (F_DARWIN || F_LINUX) */

#if	CF_USEMORE && (F_DARWIN || F_LINUX)
local int tm_getoff(con TM *tmp,intp lp) noex {
	int		rs = SR_OK ;
	if (tmp->tm_isdst >= 0) {
	    cint val = conv<int>(tmp->tm_gmtoff) ;
	    if (lp) *lp = val ;
	} else {
    	    rs = gmoff(lp) ;
	}
	return rs ;
} /* end */
#else /* CF_USEMODE */
local int tm_getoff(con TM *,intp lp) noex {
    	return gmoff(lp) ;
} /* end */
#endif /* CF_USEMORE && (F_DARWIN || F_LINUX) */

int offer::operator () (intp lp) noex {
    	int		rs = SR_OK ;
	if (finit) {
	    if (lp) *lp = (neg offval) ; /* ± mins west of GMY */
	} else {
	    if ((rs = getznlen()) >= 0) {
	        tmtime::znlen = rs ;
	        if (TIMEB tb ; (rs = uc_ftime(&tb)) >= 0) {
		    if (tb.dstflag >= 0) {
		        cint adj = (tb.dstflag > 0) ? (neg OneHour) : 0 ;
		        offval = ((tb.timezone * 60) + adj) ;
		    } /* end if (have a time-zone) */
		    if (lp) *lp = (neg offval) ; /* ± mins west of GMY */
		    finit = true ;
	        } /* end if (TIMEB) */
	    } /* end if (getznlen) */
	} /* end if (needed) */
	return rs ;
} /* end method (offer::operator) */

local int initer() noex {
    	return gmoff() ;
} /* end subroutine (initer) */

local int getznlen() noex {
    	int		rs ;
	if ((rs = znlen) < 0) {
	    ulogerror("tmtime",rs,"getznlen") ;
	} /* end if (znlen) */
	return rs ;
} /* end subroutine (getznlen) */


