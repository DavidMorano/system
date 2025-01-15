/* dater_setkey SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* handle parsing a supplied date string using a key format identifier */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Although there was probably something that could have done
	with parsing job (with a different argument syntax) I wrote
	this from scratch (sigh).  Yes, we try and avoid writing
	whatever we can. This subroutine extends the DATE object
	to parse strings with the date type indicated by a key name.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

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
	int dater_setkey(dater *dp,cc *dbuf,int dlen,TIMEB *nowp,cc *zn) noex

	Arguments:
	dp		pointer to DATE object (already initialized)
	dbuf		date buffer pointer w/ specified key=value pair
	dlen		date buffer length
	nowp		pointer to a timeb structure representing NOW
	zn		time zone name

	Returns:
	>=0		it all worked out OK
	<0		the date could not be parsed (system-return)

******************************************************************************/

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


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct datehelp {
	dater *dp ;
	cc *dsp ;
	int dsl ;
	TIMEB *nowp ;
	cc *zn ;
	datehelp(dater *p,cc *asp,int asl,TIMEB *tp,cc *azn) noex {
	    dp = p ;
	    dsp = asp ;
	    dsl = asl ;
	    nowp = tp ;
	    zn = azn ;
	} ;
	operator int () noex ;
	int exec(int,cc *,int) noex ;
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

int dater_setkey(dater *dp,cc *dsp,int dsl,TIMEB *nowp,cc *zn) noex {
    	cnullptr	np{} ;
	int		rs  ;
	if ((rs = dater_magic(dp,dsp,nowp,zn)) >= 0) {
	    rs = SR_INVALID ;
	    if (dsl < 0) dsl = strlen(dsp) ;
	    if (dsp[0]) {
		datehelp	dh(dp,dsp,dsl,nowp,zn) ;
		rs = dh ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setkey) */


/* local subroutines */

datehelp::operator int () noex {
    	int		rs = SR_OK ;

	return rs ;
}
/* end method (datehelp::operator) */

int datehelp::keyidx() noex {
    	int		rs = SR_OK ;
            int		tlen = 0 ;
            int		ti = -1 ;
            cchar	*tnam = nullptr ;
            cchar	*sp ;
	    rs = SR_OK ;
    	    /* get the key name first (if it has one) */
            if (cc *tp ; (tp = strnchr(dsp,dsl,'=')) != np) {
                sp = (tp + 1) ;
                tlen = sfshrink(dbuf,(cp - dbuf),&tnam) ;
            } else {
                bool        f = false ;
                sp = dsp ;
                tlen = sfshrink(dsp,dsl,&tnam) ;
                if ((tolc(tnam[0]) == 'c') || (tolc(tnam[0]) == 'n')) {
                    f = ((ti = matstr(datetypes,tnam,tlen)) >= 0) ;
                }
                if (! f) {
                    tlen = -1 ;
                }
            } /* end if (getting possible tnam) */
    	    /* if no type name, assume a TOUCHT type date string */
            if (ti < 0) {
                if (tlen > 0) {
                    ti = matstr(datetypes,tnam,tlen) ;
                    if (ti < 0) rs = SR_INVALID ;
                } else {
                    ti = datetype_toucht ;
                }
            } /* end if (had to find the type index) */
            if (rs >= 0) {
		rs = dater_setkeyexec(dp,ti,sp,sl,zn) ;
            } /* end if */



int datehelp::exec(int ti,cc *sp,int sl) noex {
    	int		rs = SR_OK ;
        switch (ti) {
        case datetype_touch:
            rs = dater_settouch(dp,sp,sl) ;
            break ;
        case datetype_tt:
        case datetype_ttouch:
        case datetype_toucht:
            rs = dater_settoucht(dp,sp,sl) ;
            break ;
        case datetype_log:
        case datetype_logz:
            rs = dater_setlogz(dp,sp,sl) ;
            break ;
        case datetype_strdig:
            rs = dater_setstrdig(dp,sp,sl) ;
            break ;
        case datetype_current:
        case datetype_now:
            {
                custime		t = nowp->time ;
                cint		zoff = nowp->timezone ;
                cint		isdst = nowp->dstflag ;
                rs = dater_settimezon(dp,t,zoff,zn,isdst) ;
            }
            break ;
        default:
            rs = SR_UNATCH ;
            break ;
        } /* end switch */
	return rs ;
}
/* end method (datehelp::exec) */


