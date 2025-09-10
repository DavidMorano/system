/* termtime_main (termtime) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* display a time-stamp on the terminal */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2004-06-24, David A­D­ Morano
	I rewrote this from scratch.  The previous version of this
	program was a hack.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

	Name:
	termtime

	Description:
	This program prints out the time-of-day to the upper right
	corner of the terminal display.

	Synopsis:
	$ termtime

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdio>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usysutility.hh>	/* |libu::snprintf(3u)| */
#include	<uclibmem.h>
#include	<uctimeconvx.h>		/* |uc_localtime(3uc)| */
#include	<ucstrftime.h>		/* |uc_strftime(3uc)| */
#include	<getfdfile.h>		/* |FD_STDERR| */
#include	<strmgr.h>
#include	<libf.h>		/* |fwrite(3f)| */
#include	<ascii.h>
#include	<matstr.h>
#include	<exitcodes.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| + |COLUMNS| */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |getlenstr(3u)| */
import uconstants ;			/* |sysword(3u)| + |varname(3u)| */
import ulibvals ;			/* |ulibval(3u)| */
import debug ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif


/* imported namespaces */

using libu::snprintf ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* external variables */


/* forward references */

local int	mktermstr(strmgr *) noex ;
local int	mktimestr(char *,int) noex ;
local int	mkcurstr(strmgr *,int,int) noex ;
local int	mkcurpos(strmgr *,int) noex ;
local int	gettermtype(cchar *,int = -1) noex ;


/* local variables */

enum terms {
    	term_vt100,
    	term_vt101,
    	term_vt220,
    	term_vt240,
    	term_vt320,
    	term_vt340,
    	term_vt420,
    	term_vt440,
    	term_vt520,
    	term_vt540,
	term_xterm,
	term_xtermcolor,
    	term_overlast
} ; /* end enum (terms) */

constexpr cpcchar	terms[] = {
	[term_vt100]		= "vt100",
	[term_vt101]		= "vt101",
	[term_vt220]		= "vt220",
	[term_vt240]		= "vt240",
	[term_vt320]		= "vt320",
	[term_vt340]		= "vt340",
	[term_vt420]		= "vt420",
	[term_vt440]		= "vt440",
	[term_vt520]		= "vt520",
	[term_vt540]		= "vt540",
	[term_xterm]		= "xterm",
	[term_xtermcolor]	= "xterm-color",
	[term_overlast] 	= nullptr
} ; /* end array (terms) */

enum termtypes {
    	termtype_vt,
    	termtype_ansi,
    	termtype_overlast
} ; /* end enum (termtypes) */

struct srcode {
    	cchar	*cursav ;
    	cchar	*curres ;
} ; 

constexpr srcode	termcodes[] = {
    	[termtype_vt]		= { "\e7", "\e8" },
    	[termtype_ansi]		= { "\e[s", "\e[u" },
	[termtype_overlast]	= { "\0", "\0" }
} ;

local cint	pagesz = ulibval.pagesz ;

cint		termcols = COLUMNS ;

cbool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int main(int,mainv,mainv) {
    	constexpr int	dfd = (f_debug) ? FD_STDERR : -1 ;
    	cnothrow	nt{} ;
	cnullptr	np{} ;
	FILE		*fp = stdout ;
	int		rs ;
	int		rs1 ;
	int		ex = EX_SUCCESS ;
	debfd(dfd) ;
	debprintf(__func__,"ent\n") ;
	if ((rs = pagesz) >= 0) {
	    cint tlen = rs ;
	    if (char *tbuf ; (tbuf = new(nt) char[tlen + 1]) != np) {
		if (strmgr sm ; (rs = sm.start(tbuf,tlen)) >= 0) {
		    if ((rs = mktermstr(&sm)) >= 0) {
			if (cc *cp ; (rs = sm.get(&cp)) >= 0) {
			    rs = fwrite(fp,cp,rs) ;
			} /* end if (sm.get) */
		    } /* end if (mktermstr) */
		    rs1 = sm.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (strmgr) */
		delete [] tbuf ;
	    } /* end if (m-a-f) */
	} /* end if (pagesz) */
	if_constexpr (f_debug) {
	    debprintf(__func__,"ret rs=%d\n",rs) ;
	}
	if ((ex == EX_SUCCESS) && (rs < 0)) ex = EX_DATAERR ;
	return ex ;
} /* end subroutine (main) */


/* local subroutines */

local int mktermstr(strmgr *smp) noex {
	static cchar	*valp = getenv(varname.term) ;
	cint		slen = TIMEBUFLEN ;
	int		rs = SR_NOTFOUND ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (valp) {
	    if (char sbuf[slen + 1] ; (rs = mktimestr(sbuf,slen)) >= 0) {
	        static cint rst = gettermtype(valp) ;
		cint	sl = rs ;
	        if ((rs = rst) >= 0) {
		    cint tt = rs ;
		    if ((rs = mkcurstr(smp,tt,0)) >= 0) {
			len += rs ;
			if ((rs = mkcurpos(smp,sl)) >= 0) {
			    len += rs ;
	                    rs = smp->str(sbuf,sl) ;
			    len += rs ;
			} /* end if (mkcurpos) */
			rs1 = mkcurstr(smp,tt,1) ;
			if (rs >= 0) rs = rs1 ;
			len += rs1 ;
		    } /* end if (mkcurstr) */
	        } /* end if (valid) */
	    } /* end if (mktimestr) */
	} /* end if (getenv) */
	if_constexpr (f_debug) {
	    debprintf(__func__,"ret rs=%d\n",rs) ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (mktermstr) */

local int mktimestr(char *tbuf,int tlen) noex {
    	custime		dt = time(nullptr) ;
    	int		rs ;
	int		tl = 0 ; /* return-value */
	if (TM ts ; (rs = uc_localtime(&dt,&ts)) >= 0) {
	    cchar *fmt = " %a %e %H:%M" ;
	    rs = uc_strftime(tbuf,tlen,fmt,&ts) ;
	    tl = rs ;
	} /* end if (uc_localtime) */
	if_constexpr (f_debug) {
	    debprintf(__func__,"ret rs=%d\n",rs) ;
	}
	return (rs >= 0) ? tl : rs ;
} /* end subroutine (mktimestr) */

local int mkcurstr(strmgr *smp,int tt,int w) noex {
	int		rs = SR_INVALID ;
	if ((tt >= 0) && (tt < termtype_overlast)) {
	    const srcode	*srp = (termcodes + tt) ;
	    if ((w >= 0) && (w < 2)) {
		cchar	*curstr = nullptr ;
		rs = SR_BUGCHECK ;
		switch (w) {
		case 0:
		    curstr = srp->cursav ;
		    break ;
		case 1:
		    curstr = srp->curres ;
		    break ;
		} /* end switch */
		if (curstr) {
		    rs = smp->str(curstr) ;
		}
	    } /* end if (valid) */
	} /* end if (valid) */
	if_constexpr (f_debug) {
	    debprintf(__func__,"ret rs=%d\n",rs) ;
	}
	return rs ;
} /* end subroutine (mkcurstr) */

local int mkcurpos(strmgr *smp,int sl) noex {
    	int		clen = TIMEBUFLEN ;
    	int		rs = SR_OK ;
	char		cbuf[clen + 1] ;
	cchar		*fmt = "\e[%u;%uH" ;
	cint		hpos = ((termcols + 1) - sl) ;
	if ((rs = snprintf(cbuf,clen,fmt,1,hpos)) >= 0) {
	    rs = smp->str(cbuf,rs) ;
	} /* end if (snwprintf) */
	if_constexpr (f_debug) {
	    debprintf(__func__,"ret rs=%d\n",rs) ;
	}
	return rs ;
} /* end subroutine (mkcurpos) */

local int gettermtype(cchar *sp,int µsl) noex {
    	int		rs = SR_FAULT ;
    	int		tt = 0 ; /* return-value */
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    rs = SR_NOTFOUND ;
	    if (int mi ; (mi = matostr(terms,2,sp,sl)) >= 0) {
		rs = SR_OK ;
	        switch (mi) {
	        case term_vt100:
	        case term_vt101:
	        case term_vt220:
	        case term_vt240:
	        case term_vt320:
	        case term_vt340:
	        case term_vt420:
	        case term_vt440:
	        case term_vt520:
	        case term_vt540:
		    tt = termtype_vt ;
		    break ;
	        case term_xterm:
	        case term_xtermcolor:
    		    tt = termtype_vt ;
		    break ;
	        default:
		    rs = SR_BUGCHECK ;
		    break ;
	        } /* end switch */
	    } /* end if (matostr) */
	} /* end if (getlenstr) */
	if_constexpr (f_debug) {
	    debprintf(__func__,"ret rs=%d tt=%d\n",rs,tt) ;
	}
	return (rs >= 0) ? tt : rs ;
} /* end subroutine (gettermtype) */


