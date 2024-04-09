/* mklogid SUPPORT */
/* lang=C++20 */

/* make a log ID */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-11-01, David A­D­ Morano
	This was originally written but modeled after a version of
	this sort of subroutine that I previously wrote.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mklogid

	Description:
	Make a log ID (like for use in logging).

	Synopsis:
	int mklogid(char *rbuf,int rlen,cchar *sp,int sl,int v) noex

	Arguments:
	rbuf		buffer to hold result
	rlen		length of supplied result buffer
	sp		string to use for leading part of ID
	sl		length of string to use
	v		number for trailing part of ID
	
	Returns:
	>=0		length of created ID string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<algorithm>		/* for |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sbuf.h>
#include	<ctdec.h>
#include	<ndigit.h>
#include	<ipow.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct loghelp {
	char		*rbuf ;
	cchar		*sp ;
	int		sl ;
	int		rlen ;
	int		val ;
	loghelp(char *ab,int al,cchar *asp,int asl,int v) noex {
	    rbuf = ab ;
	    sp = asp ;
	    rlen = al ;
	    sl = asl ;
	    val = v ;
	} ; /* end ctor */
	int valcvt(int,int) noex ;
	int layout(cchar *,int,int,int) noex ;
    } ; /* end struct (loghelp) */
}


/* forward references */

static int	mkmaxstrlen(int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mklogid(char *rbuf,int rlen,cchar *sp,int sl,int v) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && sp) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (v >= 0) {
	        static int	rsm = ndigit(PID_MAX,10) ;
	        if ((rs = rsm) >= 0) {
	            cint	maxdigs = rsm ;
		    if ((rs = mkmaxstrlen(maxdigs,rlen)) >= 0) {
			loghelp		lo(rbuf,rlen,sp,sl,v) ;
		        cint		maxstrlen = rs ;
			rs = lo.valcvt(maxdigs,maxstrlen) ;
			rl = rs ;
		    } /* end if (mkmaxstrlen) */
		} /* end if (ndigit) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mklogid) */


/* local subroutines */

int loghelp::valcvt(int maxdigs,int maxstrlen) noex {
	cint		dlen = DIGBUFLEN ;
	int		rs ;
	int		rl = 0 ;
	int		modval ;
	char		dbuf[DIGBUFLEN + 1] ;
	sl = strnlen(sp,sl) ;
	modval = (maxdigs < 10) ? ipow(10,maxdigs) : INT_MAX ;
	val = (val % modval) ; /* limits the decimal part to 'maxdigs' */
	if ((rs = ctdec(dbuf,dlen,val)) >= 0) {
	    cint	dl = rs ;
	    int		len ;
	    int		ml ;
	    len = (sl + dl) ;
	    ml = (len > rlen) ? (len - rlen) : 0 ;
	    {
		rs = layout(dbuf,dl,maxstrlen,ml) ;
		rl = rs ;
	    }
	} /* end if (cfdec) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (loghelp::valcvt) */

int loghelp::layout(cchar *dp,int dl,int maxstrlen,int ml) noex {
	sbuf		b ;
	int		rs ;
	int		rs1 ;
        if ((rs = b.start(rbuf,rlen)) >= 0) {
            if (ml == 0) {
                b.strw(sp,sl) ;
                b.strw(dp,dl) ;
            } else if (ml <= 2) {
                b.strw((sp + 2),(sl - 2)) ;
                b.strw(dp,dl) ;
            } else {
                sp += 2 ;
                sl -= 2 ;
                if (sl <= maxstrlen) {
                    b.strw(sp,sl) ;
                    b.strw(dp,dl) ;
                } else {
                    cint        len = max(sl,maxstrlen) + dl ;
                    ml = (len <= rlen) ? len : (len - rlen) ;
                    b.strw(sp,min(sl,maxstrlen)) ;
                    b.strw((dp + ml),(dl - ml)) ;
                } /* end if */
            } /* end if */
            rs1 = b.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (sbuf) */
	return rs ;
}
/* end method (loghelp::layout) */

static int mkmaxstrlen(int maxdigs,int rlen) noex {
	int		rs = SR_OK ;
	int		maxstrlen = 0 ;
	if (rlen > maxdigs) {
	    maxstrlen = (rlen - maxdigs) ;
	} else if (rlen < 0) {
	    maxstrlen = (LOGIDLEN - maxdigs) ;
	    rlen = LOGIDLEN ;
	} else {
	    rs = SR_OVERFLOW ;
	}
	return (rs >= 0) ? maxstrlen : rs ;
}
/* end subroutine (mkmaxstrlen) */


