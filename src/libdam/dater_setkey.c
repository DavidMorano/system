/* dater_setkey SUPPORT */
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
#include	<cstring>
#include	<usystem.h>
#include	<sfx.h>
#include	<matxstr.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"dater.h>"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


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

int dater_setkey(dater *dp,cc *dbuf,int dlen,TIMEB *nowp,cc *zn) noex {
	int		rs ;
	if ((rs = dater_magic(dp,dbuf,nowp,zn)) >= 0) {
            int		tlen = 0 ;
            int		ti = -1 ;
            cchar	*tname = nullptr ;
            cchar	*sp ;
            cchar	*cp ;
    /* get the key name first (if it has one) */
            if ((cp = strchr(dbuf,'=')) != nullptr) {
                sp = cp + 1 ;
                tlen = sfshrink(dbuf,(cp - dbuf),&tname) ;
            } else {
                bool        f = false ;
                sp = dbuf ;
                tlen = sfshrink(dbuf,-1,&tname) ;
                if ((tolc(tname[0]) == 'c') ||
                    (tolc(tname[0]) == 'n')) {
                    f = ((ti = matstr(datetypes,tname,tlen)) >= 0) ;
                }
                if (! f) {
                    tlen = -1 ;
                }
            } /* end if (getting possible tname) */
    /* if there was no type name, assume it was a TOUCHT type date string */
            if (ti < 0) {
                if (tlen > 0) {
                    ti = matstr(datetypes,tname,tlen) ;
                    if (ti < 0) rs = SR_INVALID ;
                } else {
                    ti = datetype_toucht ;
                }
            } /* end if (had to find the type index) */
            if (rs >= 0) {
                switch (ti) {
                case datetype_touch:
                    rs = dater_settouch(dp,sp,-1) ;
                    break ;
                case datetype_tt:
                case datetype_ttouch:
                case datetype_toucht:
                    rs = dater_settoucht(dp,sp,-1) ;
                    break ;
                case datetype_log:
                case datetype_logz:
                    rs = dater_setlogz(dp,sp,-1) ;
                    break ;
                case datetype_strdig:
                    rs = dater_setstrdig(dp,sp,-1) ;
                    break ;
                case datetype_current:
                case datetype_now:
                    {
                        const time_t	t = nowp->time ;
                        cint	zoff = nowp->timezone ;
                        cint	isdst = nowp->dstflag ;
                        rs = dater_settimezon(dp,t,zoff,zn,isdst) ;
                    }
                    break ;
                default:
                    rs = SR_UNATCH ;
                    break ;
                } /* end switch */
            } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dater_setkey) */


