/* fmtsub3 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

*/

/* Copyright © 1998,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtsub

	Description:
	This (FMTSUB) is a helper object for the FMTSTR facility.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdint>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<cwchar>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<ascii.h>		/* LIBU |CH_{x}| */
#include	<strn.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"fmtstr.h"
#include	"fmtutil.hh"
#include	"fmtsub.hh"

#pragma		GCC dependency		"mod/uconstants.ccm"
#pragma		GCC dependency		"mod/findbit.ccm"

module fmtsub ;

import uconstants ;			/* |numbasedig(3u)| */
import findbit ;			/* |f{x}b{c}(3u)| */
import fmtutil ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo_fl {
	uint		left:1 ;
	uint		zerofill:1 ;
	uint		plsign:1 ;
	uint		misign:1 ;
	uint		pmsign:1 ;
	uint		ismin:1 ;
	uint		isdig:1 ;
	uint		istru:1 ;
    } ; /* end struct (subinfo_fl) */
    struct subinfo {
	fmtsub		*op ;
	fmtspec		*fsp ;
	cchar		*sp ;
	int		sl ;
	int		fcode ;
	int		width ;
	int		prec ;
	int		npad = 0 ;
	subinfo_fl	fl{} ;
	subinfo(fmtsub *p,fmtspec *s,cc *asp,int asl) noex : op(p), fsp(s) {
	    sp = asp ;
	    sl = asl ;
	} ; /* end ctor */
	operator int () noex ;
	int loadvals	() noex ;
	int loadflags	() noex ;
	int calcfill	() noex ;
	int calcsign	() noex ;
	int adj1	() noex ;
	int adj2	() noex ;
	int calcpad	() noex ;
	int putpad	() noex ;
	int putsign	() noex ;
	int putalt	() noex ;
	int putzerofill	() noex ;
	int putprecfill	() noex ;
	int putstr	() noex ;
	int putfin	() noex ;
    } ; /* end struct (subinfo) */
    typedef int (subinfo::*subinfo_m)() noex ;
} /* end namespace */


/* forward references */


/* local variables */

constexpr subinfo_m	subs[] = {
	&subinfo::loadvals,
	&subinfo::loadflags,
	&subinfo::calcfill,
	&subinfo::calcsign,
	&subinfo::adj1,
	&subinfo::adj2,
	&subinfo::calcpad,
	&subinfo::putpad,
	&subinfo::putsign,
	&subinfo::putalt,
	&subinfo::putzerofill,
	&subinfo::putprecfill,
	&subinfo::putstr,
	&subinfo::putfin
} ; /* end array (subs) */

local cchar		mincodes[]	= "pxPX" ;
local cchar		digcodes[]	= "aboiduxpefgABEGPX" ;
local cchar		trucodes[]	= "rsRS" ;
local cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int fmtsub_emit(fmtsub *op,fmtspec *fsp,cchar *sp,int µsl) noex {
	int		rs = SR_BUGCHECK ;
	if (op && fsp) ylikely {
	    rs = SR_OK ;
	    if (sp && (fsp->fcode > 0)) {
		if (int sl = getlenstr(sp,µsl) ; sl >= 0) {
		    subinfo si(op,fsp,sp,sl) ;
		    rs = si ;
		} /* end if (getlenstr) */
	    } /* end if (active) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (fmtsub_emit) */


/* local subroutines */

subinfo::operator int () noex {
    	int		rs = SR_OK ;
	DPRINTF("ent\n") ;
	for (cauto &m : subs) {
	    rs = (this->*m)() ;
	    if (rs < 0) break ;
	} /* end for */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (subinfo::operator) */

int subinfo::loadvals() noex {
    	int		rs = SR_OK ;
	fcode		= fsp->fcode ;
	width		= fsp->width ;
	prec		= fsp->prec ;
	DPRINTF("ret rs=%d fcode=>%c<\n",rs,fcode) ;
	return rs ;
} /* end method (subinfo::loadvals) */

int subinfo::loadflags() noex {
    	int		rs = SR_OK ;
	fl.left		= fsp->fl.left ;
	fl.zerofill	= fsp->fl.zerofill ;
	fl.plsign	= fsp->fl.plsign ;
	fl.ismin	= (strchr(mincodes,fcode) != nullptr) ;
	fl.isdig	= fl.ismin || (strchr(digcodes,fcode) != nullptr) ;
	fl.istru	= fl.isdig || (strchr(trucodes,fcode) != nullptr) ;
	return rs ;
} /* end method (subinfo::loadflags) */

local int subinfo_minfill(subinfo *sip) noex {
	fmtsub		*op	= sip->op ;
	fmtspec		*fsp	= sip->fsp ;
    	int		rs = SR_OK ;
	int		sz = 0 ;
	int		mfill = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (fsp->lenmod >= 0) {
	    DPRINTF("type-size lenmod=%d\n",fsp->lenmod) ;
	    switch (fsp->lenmod) {
	    case lenmod_halfhalf:
		sz = szof(char) ;
		break ;
	    case lenmod_half:
		sz = szof(short) ;
		break ;
	    case lenmod_long:
		sz = szof(long) ;
		break ;
	    case lenmod_longlong:
		sz = szof(longlong) ;
		break ;
	    } /* end switch */
	} else {
	    DPRINTF("type-size def\n") ;
	    sz = szof(int) ;
	} /* end if (had a length modifier) */
	if (sz > 0) {
	    cint tc = ffbs(sz) ;
	    DPRINTF("type-code=%d\n",tc) ;
	    mfill = numbasedig.digs[tc][op->numbase] ;
	} /* end if */
	DPRINTF("ret rs=%d mfill=%d\n",rs,mfill) ;
	return (rs >= 0) ? mfill : rs ;
} /* end subroutine (subinfo_minfill) */

int subinfo::calcfill() noex {
	int		rs = SR_OK ;
	DPRINTF("ent fcode=>%c<\n",fcode) ;
	switch (fcode) {
	case 'b':
	case 'p':
	case 'x':
	case 'B':
	case 'P':
	case 'X':
	    if (op->fl.mminfill && (width < 0)) {
	        if ((rs = subinfo_minfill(this)) >= 0) {
	            width = rs ;
	            fl.zerofill = true ;
	        } /* end if (subinfo_minfill) */
	    } /* end if (mingill option) */
	    break ;
	} /* end switch */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (subinfo::calcfill) */

int subinfo::calcsign() noex {
    	int		rs = SR_OK ;
	if ((sl > 0) && fl.isdig) {
	    cbool f_p = (*sp == '+') ;
	    cbool f_m = (*sp == '-') ;
	    if (f_p || f_m) {
	        fl.plsign = f_p ;
	        fl.misign = f_m ;
	        sp += 1 ;
	        sl -= 1 ;
	    }
	} /* end if */
	return rs ;
} /* end method (subinfo::calcsign) */

int subinfo::adj1() noex {
    	int		rs = SR_OK ;
	if (prec >= 0) {
	    if (sl > prec) {
	        if (fl.istru) {
	            sp += (sl - prec) ; /* truncate on left */
	            sl -= (sl - prec) ;
	        } else {
	            sl = prec ; /* truncate on right */
	        }
	    }
	} /* end if (have precision) */
	return rs ;
} /* end method (subinfo::adj1) */

/* calculate the minimum field width */
int subinfo::adj2() noex {
    	int		rs = SR_OK ;
	int		ml = 0 ;
	DPRINTF("ent width=%d\n",width) ;
	if (fl.isdig) ml = 1 ;
	if ((! op->fl.mminfill) && (sl > ml)) {
	    ml = sl ;
	}
	if ((prec >= 0) && (prec > ml)) {
	    ml = prec ;
	}
	if (fl.plsign || fl.misign) {
	    ml += 1 ;
	}
	if (ml > width) {
	    width = ml ;
	}
	DPRINTF("ret rs=%d width=%d\n",rs,width) ;
	return rs ;
} /* end method (subinfo::adj2) */

/* calculate any padding (blanks or zero-fills) */
int subinfo::calcpad() noex {
    	int		rs = SR_OK ;
	fl.pmsign = (fl.plsign || fl.misign) ;
	{
	    int ml = sl ;
	    if ((prec >= 0) && (prec > sl)) {
		ml = prec ;
	    }
	    if (fl.pmsign) {
		ml += 1 ;
	    }
	    if (width > ml) {
		npad = (width - ml) ;
	    }
	} /* end block */
	DPRINTF("ret rs=%d width=%d\n",rs,width) ;
	return rs ;
} /* end method (subinfo::calcpad) */

/* print out any leading padding (field width) */
int subinfo::putpad() noex {
    	int		rs = SR_OK ;
	if ((! fsp->fl.left) && (! fl.zerofill)) {
	    if (npad > 0) {
	        rs = fmtsub_blanks(op,npad) ;
	    }
	} /* end if */
	return rs ;
} /* end method (subinfo::putpad) */

/* we may want to print a leading '-' before anything */
int subinfo::putsign() noex {
    	int		rs = SR_OK ;
	if (fl.pmsign) {
	    cint	ch = (fl.misign) ? '-' : '+' ;
	    rs = fmtsub_chr(op,ch) ;
	    width -= 1 ;
	} /* end if */
	return rs ;
} /* end method (subinfo::putsign) */

/* output any alternate character sequences */
int subinfo::putalt() noex {
    	int		rs = SR_OK ;
	if (fsp->fl.alternate) {
	    switch (fcode) {
	    case 'x':
	    case 'X':
	    case 'p':
	    case 'P':
	        rs = fmtsub_strw(op,"0x",2) ;
	        break ;
	    case 'o':
	        if ((sl == 0) || (sp[0] != '0')) {
	            rs = fmtsub_chr(op,'0') ;
	        }
	        break ;
	    case 'b':
	    case 'B':
	        rs = fmtsub_strw(op,"0b",2) ;
	        break ;
	    } /* end switch */
	} /* end if */
	return rs ;
} /* end method (subinfo::putalt) */

/* any zero-fill due to field width */
int subinfo::putzerofill() noex {
    	int		rs = SR_OK ;
	if ((! fsp->fl.left) && fl.zerofill && (npad > 0)) {
	    cint ch = (fl.isdig ? '0' : ' ') ;
	    for (int i = 0 ; (rs >= 0) && (i < npad) ; i += 1) {
	        rs = fmtsub_chr(op,ch) ;
	    } /* end for */
	} /* end if */
	return rs ;
} /* end method (subinfo::putzerofill) */

/* send out any filling due to precision */
int subinfo::putprecfill() noex {
    	int		rs = SR_OK ;
	if ((prec >= 0) && (prec > sl)) {
	    cint ch = (fl.isdig ? '0' : ' ') ;
	    for (int i = 0 ; (rs >= 0) && (i < (prec - sl)) ; i += 1) {
	        rs = fmtsub_chr(op,ch) ;
	    } /* end for */
	} /* end if */
	return rs ;
} /* end method (subinfo::putprecfill) */

/* send out the string itself */
int subinfo::putstr() noex {
    	int		rs = SR_OK ;
	if (sl > 0) {
	    if (fl.ismin) { /* truncation for HEX */
	        if ((width >= 0) && (sl > width)) {
	            cint	skip = (sl - width) ;
	            sp += skip ;
	            sl -= skip ;
	        } /* end if (truncation indicated) */
	    } /* end if (hex-value) */
	    if (rs >= 0) {
		if (fl.isdig) {
	            rs = op->strw(sp,sl) ;
		} else {
	            rs = op->strclean(sp,sl) ;
		}
	    } /* end if (ok) */
	} /* end if (non-zero postive) */
	return rs ;
} /* end method (subinfo::putstr) */

/* send out trailing pad characters */
int subinfo::putfin() noex {
	int		rs = SR_OK ;
	if (fsp->fl.left && (npad > 0)) {
	    rs = fmtsub_blanks(op,npad) ;
	} /* end if */
	return rs ;
} /* end method (subinfo::putfin) */


