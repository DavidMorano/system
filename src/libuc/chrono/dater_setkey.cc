/* dater_setkey SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle parsing a supplied date string using a key format identifier */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Although there was probably something that could have been
	done with the parsing of date specification strins (here
	below) using some existing code (perhaps with a different
	argument syntax) I wrote this (below) from scratch (sigh).
	Yes, I try to avoid writing anything new whatever I can.
	This subroutine extends the DATER object to parse strings
	with the date type indicated by a key name.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dater_setkey

	Description:
	This subroutine will parse out a date that has been specified
	with a syntax like:
		type=string
	where:
		type		is a name of a date string type
		string		is the string that is supposed to be the date

	Synopsis:
	int dater_setkey(dater *op,cc *dbuf,int dlen,TIMEB *nowp,cc *zn) noex

	Arguments:
	op		pointer to DATE object (already initialized)
	dbuf		date buffer pointer w/ specified key=value pair
	dlen		date buffer length
	nowp		pointer to a timeb structure representing NOW
	zn		time zone name

	Returns:
	>=0		it all worked out OK
	<0		the date could not be parsed (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sfx.h>
#include	<strn.h>
#include	<matstr.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"dater.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct datehelp {
	dater		*op ;
	cc		*dsp ;
	TIMEB		*nowp ;
	cc		*zn ;
	int		dsl ;
	datehelp(dater *p,cc *asp,int asl,TIMEB *tp,cc *azn) noex : op(p) {
	    dsp = asp ;
	    dsl = asl ;
	    nowp = tp ;
	    zn = azn ;
	} ;
	operator int () noex ;
	int prockey(int,cc *,int) noex ;
	int procval(cc *,int) noex ;
    } ; /* end struct (datehelp) */
}


/* forward references */


/* local variables */

enum datetypes {
	datetype_current,
	datetype_now,
	datetype_touch,
	datetype_tt,
	datetype_ttouch,
	datetype_toucht,
	datetype_log,
	datetype_logz,
	datetype_strdig,
	datetype_overlast
} ;

constexpr cpcchar	datetypes[] = {
	"current",
	"now",
	"touch",
	"tt",
	"ttouch",
	"toucht",
	"log",
	"logz",
	"strdig",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int dater_setkey(dater *op,cc *dsp,int dsl,TIMEB *nowp,cc *zn) noex {
	int		rs  ;
	if ((rs = dater_magic(op,dsp,nowp,zn)) >= 0) {
	    rs = SR_INVALID ;
	    if (dsl < 0) dsl = lenstr(dsp) ;
	    if (dsp[0]) {
		datehelp dh(op,dsp,dsl,nowp,zn) ;
		rs = dh ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setkey) */


/* local subroutines */

datehelp::operator int () noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
        if (cc *tp ; (tp = strnchr(dsp,dsl,'=')) != np) {
	    cint	vall = intconv((dsp + dsl) - (tp + 1)) ;
	    cint	tl = intconv(tp - dsp) ;
            cc		*valp = (tp + 1) ;
	    cchar	*kp ;
            if (int kl ; (kl = sfshrink(dsp,tl,&kp)) > 0) {
                if (int ti ; (ti = matstr(datetypes,kp,kl)) >= 0) {
		    rs = prockey(ti,valp,vall) ;
		} else {
		    rs = SR_NOTSUP ;
		}
	    } else {
		rs = procval(valp,vall) ;
	    }
        } else {
	    rs = procval(dsp,dsl) ;
	}
	return rs ;
}
/* end method (datehelp::operator) */

int datehelp::procval(cchar *sp,int sl) noex {
    	int		rs = SR_OK ;
	cchar		*cp ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	    cint	ch = tolc(cp[0]) ;
            if ((ch == 'c') || (ch == 'n')) {
		if (int ti ; (ti = matstr(datetypes,cp,cl)) >= 0) {
		    rs = prockey(ti,cp,cl) ;
		} else {
		    rs = SR_BADFMT ;
		}
            } else {
		cint	ti = datetype_toucht ;
		rs = prockey(ti,cp,cl) ;
	    }
	} else {
	    rs = SR_NOMSG ;
	}
	return rs ;
}
/* end method (datehelp::procval) */

int datehelp::prockey(int ti,cc *sp,int sl) noex {
    	int		rs = SR_NOTSUP ;
        switch (ti) {
        case datetype_touch:
            rs = dater_settouch(op,sp,sl) ;
            break ;
        case datetype_tt:
        case datetype_ttouch:
        case datetype_toucht:
            rs = dater_settoucht(op,sp,sl) ;
            break ;
        case datetype_log:
        case datetype_logz:
            rs = dater_setlogz(op,sp,sl) ;
            break ;
        case datetype_strdig:
            rs = dater_setstrdig(op,sp,sl) ;
            break ;
        case datetype_current:
        case datetype_now:
            {
                custime		t = nowp->time ;
                cint		zoff = nowp->timezone ;
                cint		isdst = nowp->dstflag ;
                rs = dater_settimezon(op,t,zoff,zn,isdst) ;
            }
            break ;
        } /* end switch */
	return rs ;
}
/* end method (datehelp::prockey) */


