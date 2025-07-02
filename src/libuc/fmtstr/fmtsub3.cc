/* fmtsub3 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */


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
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstdarg>
#include	<cstring>
#include	<cwchar>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>		/* system-returns + |uc_malloc(3uc)| */
#include	<stdintx.h>
#include	<ascii.h>		/* |CH_{x}| */
#include	<strn.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

module fmtsub ;

import fmtstrdata ;
import fmtutil ;
import fmtspec ;

/* local defines */



/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo_fl {
	uint		left:1 ;
	uint		zerofill:1 ;
	uint		plus:1 ;
	uint		plusminus:1 ;
	uint		truncleft:1 ;
	uint		isdigital:1 ;
	uint		specialhex:1 ;
	uint		minus:1 ;
    } ;
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
	} ;
	operator int () noex ;
	int loadvals() noex ;
	int loadflags() noex ;
	int calctrunc() noex ;
	int calcdigit() noex ;
	int calcfill() noex ;
	int calcsign() noex ;
	int adj1() noex ;
	int adj2() noex ;
	int adj3() noex ;
	int adj4() noex ;
	int adj5() noex ;
	int alternate() noex ;
	int zerofill() noex ;
	int precfill() noex ;
	int strput() noex ;
	int putpad() noex ;
    } ; /* end struct (subinfo) */
    typedef int (subinfo::*subinfo_m)() noex ;
} /* end export */


/* forward references */


/* local variables */

constexpr subinfo_m	mems[] = {
	&subinfo::loadvals,
	&subinfo::loadflags,
	&subinfo::calctrunc,
	&subinfo::calcdigit,
	&subinfo::calcfill,
	&subinfo::calcsign,
	&subinfo::adj1,
	&subinfo::adj2,
	&subinfo::adj3,
	&subinfo::adj4,
	&subinfo::adj5,
	&subinfo::alternate,
	&subinfo::zerofill,
	&subinfo::precfill,
	&subinfo::strput,
	&subinfo::putpad
} ;

/* exported variables */


/* exported subroutines */

int fmtsub_emit(fmtsub *op,fmtspec *fsp,cchar *sp,int sl) noex {
	int		rs = SR_BUGCHECK ;
	if (op && fsp) {
	    rs = SR_OK ;
	    if (sp && (fsp->fcode > 0)) {
		if (sl < 0) sl = lenstr(sp) ;
		{
		    subinfo si(op,fsp,sp,sl) ;
		    rs = si ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (fmtsub_emit) */

subinfo::operator int () noex {
    	int		rs = SR_OK ;
	for (auto &m : mems) {
	    rs = (this->*m)() ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end method (subinfo::operator) */

int subinfo::loadvals() noex {
    	int		rs = SR_OK ;
	fcode = fsp->fcode ;
	width = fsp->width ;
	prec = fsp->prec ;
	return rs ;
} /* end method (subinfo::loadvals) */

int subinfo::loadflags() noex {
    	int		rs = SR_OK ;
	fl.left = fsp->fl.left ;
	fl.zerofill = fsp->fl.zerofill ;
	fl.plus = fsp->fl.plus ;
	fl.plusminus = false ;
	fl.truncleft = false ;
	fl.isdigital = false ;
	fl.specialhex = false ;
	return rs ;
} /* end method (subinfo::loadflags) */

int subinfo::calctrunc() noex {
    	int		rs = SR_OK ;
	switch (fcode) {
	case 'S':
	case 's':
	case 't':
	case 'i':
	case 'd':
	case 'u':
	case chx_binary:
	case 'o':
	case 'X':
	case 'x':
	case 'P':
	case 'p':
	    fl.truncleft = true ;
	} /* end switch */
	return rs ;
} /* end method (subinfo::calctrunc) */

int subinfo::calcdigit() noex {
    	int		rs = SR_OK ;
	switch (fcode) {
	case 'i':
	case 'd':
	case 'u':
	case chx_binary:
	case 'o':
	case 'e':
	case 'g':
	case 'f':
	case 'E':
	case 'G':
	    fl.isdigital = true ;
	} /* end switch */
	return rs ;
} /* end method (subinfo::calcdigit) */

int subinfo::calcfill() noex {
	int		rs = SR_OK ;
	switch (fcode) {
	case chx_binary:
	case 'X':
	case 'x':
	case 'P':
	case 'p':
	    fl.specialhex = true ;
	    if (width < 0) {
	        width = sl ;
	        fl.zerofill = true ;
	    }
	} /* end switch */
	return rs ;
} /* end method (subinfo::calcfill) */

int subinfo::calcsign() noex {
    	int		rs = SR_OK ;
	    if ((sl > 0) && fl.isdigital) {
	        bool f_p = (*sp == '+') ;
	        bool f_m = (*sp == '-') ;
	        if (f_p || f_m) {
	            fl.plus = f_p ;
	            fl.minus = f_m ;
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
	            if (fl.truncleft) {
	                sp += (sl-prec) ; /* truncate on left */
	                sl -= (sl-prec) ;
	            } else {
	                sl = prec ; /* truncate on right */
	            }
	        }
	    } /* end if */
	return rs ;
} /* end method (subinfo::adj1) */

/* calculate the minimum field width */
int subinfo::adj2() noex {
    	int		rs = SR_OK ;
	        int	ml = 0 ;
	        if (! fl.specialhex) ml = sl ;
	        if ((prec >= 0) && (prec > ml)) ml = prec ;
	        if (fl.plus | fl.minus) ml += 1 ;
	        if (ml > width) width = ml ;
	return rs ;
} /* end method (subinfo::adj2) */

/* calculate any padding (blanks or zero-fills) */
int subinfo::adj3() noex {
    	int		rs = SR_OK ;
	    fl.plusminus = (fl.plus || fl.minus) ;
	    {
	        int ml = sl ;
	        if ((prec >= 0) && (prec > sl)) ml = prec ;
	        if (fl.plusminus) ml += 1 ;
	        if (width > ml) npad = (width - ml) ;
	    } /* end block */
	return rs ;
} /* end method (subinfo::adj3) */

/* print out any leading padding (field width) */
int subinfo::adj4() noex {
    	int		rs = SR_OK ;
	    if ((! fsp->fl.left) && (! fl.zerofill)) {
	        if (npad > 0) {
	            rs = fmtsub_blanks(op,npad) ;
	        }
	    } /* end if */
	return rs ;
} /* end method (subinfo::adj4) */

/* we may want to print a leading '-' before anything */
int subinfo::adj5() noex {
    	int		rs = SR_OK ;
	    if (fl.plusminus) {
	        int	ch = (fl.minus) ? '-' : '+' ;
	        rs = fmtsub_chr(op,ch) ;
	        width -= 1 ;
	    } /* end if */
	return rs ;
} /* end method (subinfo::adj5) */

/* handle any alternates */
int subinfo::alternate() noex {
    	int		rs = SR_OK ;
	    if (fsp->fl.alternate) {
	        switch (fcode) {
	        case 'x':
	        case 'X':
	            rs = fmtsub_strw(op,"0x",2) ;
	            break ;
	        case 'o':
	            if (sp[0] != '0') {
	                rs = fmtsub_chr(op,'0') ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if */
	return rs ;
} /* end method (subinfo::alternate) */

/* any zero-fill due to field width */
int subinfo::zerofill() noex {
    	int		rs = SR_OK ;
	    if ((! fsp->fl.left) && fl.zerofill && (npad > 0)) {
	        int ch = (fl.isdigital ? '0' : ' ') ;
	        for (int i = 0 ; (rs >= 0) && (i < npad) ; i += 1) {
	            rs = fmtsub_chr(op,ch) ;
	        }
	    } /* end if */
	return rs ;
} /* end method (subinfo::zerofill) */

/* send out any filling due to precision */
int subinfo::precfill() noex {
    	int		rs = SR_OK ;
	    if ((prec >= 0) && (prec > sl)) {
	        int ch = (fl.isdigital ? '0' : ' ') ;
	        for (int i = 0 ; (rs >= 0) && (i < (prec - sl)) ; i += 1) {
	            rs = fmtsub_chr(op,ch) ;
	        }
	    } /* end if */
	return rs ;
} /* end method (subinfo::procfill) */

/* send out the string itself */
int subinfo::strput() noex {
    	int		rs = SR_OK ;
	    if (sl > 0) {
	        if (fl.specialhex) {
	            if ((width >= 0) && (sl > width)) {
	                int	skip = (sl - width) ;
	                sp += skip ;
	                sl -= skip ;
	            }
	        }
	        rs = fmtsub_cleanstrw(op,sp,sl) ;
	    } /* end if */
	return rs ;
} /* end method (subinfo::strput) */

/* send out trailing pad characters */
int subinfo::putpad() noex {
    	int		rs = SR_OK ;
	    if (fsp->fl.left && (npad > 0)) {
	        rs = fmtsub_blanks(op,npad) ;
	    } /* end if */
	return rs ;
} /* end method (subinfo::putpad) */


