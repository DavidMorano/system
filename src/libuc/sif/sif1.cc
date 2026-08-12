/* sif1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* String-Interate-Find (SIF) object */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2020-09-04, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Names:
	operator()
	next
	nextchr
	nextbrk
	chr
	brk

	Description:
	These subroutines allow for iterative retrieval of fields
	from a composite c-string of fields.

	Synopsis:
	int operator() 	(cchar *rpp) noex
	int next	(cchar *rpp) noex
	int nextchr	(cchar *rpp) noex
	int nextbrk	(cchar *rpp) noex
	int chr		(cchar *rpp) noex
	int brk		(cchar *rpp) noex

	Arguments:
	rpp		result pointer pointer

	Returns:
	>0		got a field
	==0		did not get a field, or got a zero-length field
	<0		did not get a field delimited by specified delimiters

	Notes:
	"next"		find white-space OR specified delimiter(s)
	"sp"		find delimeter and remove surrounding white-space
	"chr"		find delimeter only
	"brk"		find delimeter only
	"operator"	like 'next' versions

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"
#pragma		GCC dependency		"mod/deb.ccm"

module sif ;

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* |sfx(3u)| */
import deb ;

/* local defines */

#define CDEBPR(fmt, ...) \
	if_constexpr (f_debug) \
    	debprintf(__func__,fmt __VA_OPT__(,) __VA_ARGS__)

#define	ISW(ch)		char_iswht(ch)	/* LIBU */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libu::strnchr ;			/* subroutine */
using libu::strnbrk ;			/* subroutine */


/* local typedefs */


/* external subroutines */

namespace libu {
    extern "C++" char * strnwhtbrk(cchar *,int,con chrset *) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int sif::enter(cchar **rpp) noex {
	int	rs = SR_FAULT ;
	if (rpp) ylikely {
            if (! finit) {
	        if (sp) ylikely {
                    if (sl < 0) sl = lenstr(sp) ;
                    finit = true ;
		    rs = SR_OK ;
	        } 
	    } else {
		rs = SR_OK ;
            }
	} /* end if (non-null) */
	return rs ;
} /* end method (sif::enter) */

int sif::operator () (cchar **rpp) noex {
	int		rs ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	CDEBPR("ent sstr=%p\n",sstr) ;
	if (sstr) {
	CDEBPR("sstr=>%s<\n",sstr) ;
	}
	if ((rs = enter(rpp)) >= 0) ylikely {
	    if (sstr) {
		rl = nextbrk(rpp) ;
	    } else if (sch) {
		rl = nextchr(rpp) ;
	    } else {
		if ((rl = sfnext(sp,sl,&rp)) > 0) ylikely {
		    sl -= intconv((rp + rl) - sp) ;
		    sp = (rp + rl) ;
		}
	        *rpp = rp ;
	    } /* end if */
	} /* end if (enter) */
	CDEBPR("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end method (sif::operator) */

int sif::next(cchar **rpp) noex {
	int		rs ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	CDEBPR("ent\n") ;
	if ((rs = enter(rpp)) >= 0) ylikely {
	    if ((rl = sfnext(sp,sl,&rp)) > 0) {
		sl -= intconv((rp + rl) - sp) ;
		sp = (rp + rl) ;
	    }
	    *rpp = rp ;
	} /* end if (enter) */
	return (rs >= 0) ? rl : rs ;
} /* end method (sif::next) */

int sif::nextbrk(cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	CDEBPR("ent\n") ;
	if ((sstr || sch) && ((rs = enter(rpp)) >= 0)) ylikely {
            if_constexpr (f_debug) {
                strnul sr(sp,sl) ;
                CDEBPR("str sl=%d sp=>%s<\n",sl,ccp(sr)) ;
            }
            while ((sl > 0) && (rl <= 0)) {
                if_constexpr (f_debug) {
                    strnul sr(sp,sl) ;
                    CDEBPR("rem sl=%d sp=>%s<\n",sl,ccp(sr)) ;
                }
                if (cchar *tp = libu::strnwhtbrk(sp,sl,&sset) ; tp) {
                    cint tl = intconv(tp - sp) ;
                    CDEBPR("middle tl=%d\n",tl) ;
                    rl = sfshrink(sp,tl,&rp) ;
                    sl -= intconv((tp + 1) - sp) ;
                    sp = (tp + 1) ;
                    CDEBPR("middle result sl=%d rl=%d\n",sl,rl) ;
                } else {
                    if_constexpr (f_debug) {
                        strnul sr(sp,sl) ;
                        CDEBPR("fin sl=%d ss=>%s<\n",sl,ccp(sr)) ;
                    }
                    rl = sfshrink(sp,sl,&rp) ;
                    CDEBPR("sfshrink cl=%d rp=>%s<\n",rl,rp) ;
                    sp += sl ;
                    sl = 0 ;
                } /* end if */
            } /* end while */
            *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (enter) */
	CDEBPR("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end method (sif::nextbrk) */

int sif::spchr(cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	CDEBPR("ent\n") ;
	if (sch && ((rs = enter(rpp)) >= 0)) ylikely {
	    while ((sl > 0) && (rl <= 0)) {
	        if (cchar *tp = strnchr(sp,sl,sch) ; tp) {
		    cint tl = intconv(tp - sp) ;
		    rl = sfshrink(sp,tl,&rp) ;
		    sl -= intconv((tp + 1) - sp) ;
		    sp = (tp + 1) ;
		} else {
		    rl = sfshrink(sp,sl,&rp) ;
		    sp += sl ;
		    sl = 0 ;
	        } /* end if */
	    } /* end while */
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (non-null) */
	CDEBPR("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end method (sif::spchr) */

int sif::spbrk(cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	CDEBPR("ent\n") ;
	if (sstr && ((rs = enter(rpp)) >= 0)) ylikely {
	    while ((sl > 0) && (rl <= 0)) {
	        if (cchar *tp = strnbrk(sp,sl,sstr) ; tp) {
		    cint tl = intconv(tp - sp) ;
		    rl = sfshrink(sp,tl,&rp) ;
		    sl -= intconv((tp + 1) - sp) ;
		    sp = (tp + 1) ;
		} else {
		    rl = sfshrink(sp,sl,&rp) ;
		    sp += sl ;
		    sl = 0 ;
	        } /* end if */
	    } /* end while */
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (enter) */
	CDEBPR("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end method (sif::spbrk) */

int sif::chr(cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rl = SR_NOTFOUND ;	/* <- indicate not-found */
	cchar		*rp = nullptr ;
	CDEBPR("ent\n") ;
	if (sch && ((rs = enter(rpp)) >= 0)) ylikely {
	    if (cchar *tp = strnchr(sp,sl,sch) ; tp) {
		rp = sp ;
		rl = intconv(tp - sp) ;
		sl -= intconv((tp + 1) - sp) ;
		sp = (tp + 1) ;
	    } else {
		rp = sp ;
		rl = sl ;
		sp += sl ;
		sl = 0 ;
	    } /* end if */
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (enter) */
	CDEBPR("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end method (sif::chr) */

int sif::brk(cchar **rpp) noex {
	int		rs = SR_INVALID ;
	int		rl = SR_NOTFOUND ; /* return-value */
	cchar		*rp = nullptr ;
	CDEBPR("ent\n") ;
	if (sstr && ((rs = enter(rpp)) >= 0)) ylikely {
	    if (cchar *tp = strnbrk(sp,sl,sstr) ; tp) {
		rp = sp ;
		rl = intconv(tp - sp) ;
		sl -= intconv((tp + 1) - sp) ;
		sp = (tp + 1) ;
	    } else {
		rp = sp ;
		rl = sl ;
		sp += sl ;
		sl = 0 ;
	    } /* end if */
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (enter) */
	CDEBPR("ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
} /* end method (sif::brk) */


/* local subroutines */

sif_co::operator bool () noex {
	cnullptr	np{} ;
	bool		f = false ;
	if (op) ylikely {
	    switch (cint ch = mkchar(op->sp[0]) ; w) {
	    case sifmem_iswhitechr:
	        f = ISW(ch) && (ch != op->sch) ;
	        break ;
	    case sifmem_iswhitestr:
	        f = ISW(ch) && (strchr(op->sstr,ch) == np) ;
	        break ;
	    case sifmem_isspanchr:
	        f = (! ISW(ch)) && (ch != op->sch) ;
	        break ;
	    case sifmem_isspanstr:
	        f = (! ISW(ch)) && (strchr(op->sstr,ch) == np) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return f ;
} /* end method (sif_co::operator) */


