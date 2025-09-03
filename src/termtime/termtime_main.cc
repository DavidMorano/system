/* termtime_main (termtime) */
/* lang=C20 */

/* part of TERMCHAR program */
/* version %I% last-modified %G% */


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
	$ termtime [arguments]

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<uctimeconvx.h>		/* |uc_localtime(3uc)| */
#include	<ucstrftime.h>		/* |uc_strftime(3uc)| */
#include	<strmgr.h>
#include	<ascii.h>
#include	<exitcodes.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

import ulibvals ;			

/* local defines */

#ifndef	CBUFLEN
#define	CBUFLEN		100
#endif


/* external subroutines */


/* external variables */


/* external variables */


/* forward references */

local int	mktermstr(strmgr *) noex ;
local int	mktimestr(char *,int) noex ;


/* local variables */

local cint	pagesz = ulibval.pagesz ;


/* exported variables */


/* exported subroutines */

int main(int,mainv,mainv) {
    	cnothrow	nt{} ;
	cnullptr	np{} ;
	FILE		*fp = stdout ;
	int		rs ;
	int		rs1 ;
	int		ex = EX_SUCCESS ;
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
	if ((ex == EX_SUCCESS) && (rs < 0)) ex = EX_DATAERR ;
	return ex ;
} /* end subroutine (main) */


/* local subroutines */

local int mktermstr(strmgr *smp) noex {
	cint		slen = TIMEBUFLEN ;
    	int		rs ;
	if (char sbuf[slen + 1] ; (rs = mktimestr(sbuf,slen)) >= 0) {
	    rs = SR_OK ;
	    (void) smp ;


	} /* end if (mktimestr) */
	return rs ;
} /* end subroutine (mktermstr) */

local int mktimestr(char *tbuf,int tlen) noex {
    	custime		dt = time(nullptr) ;
    	int		rs ;
	int		tl = 0 ; /* return-value */
	if (TM ts ; (rs = uc_localtime(&dt,&ts)) >= 0) {
	    cchar *fmt = "%a %e %H:%M" ;
	    rs = uc_strftime(tbuf,tlen,fmt,&ts) ;
	    tl = rs ;
	} /* end if (uc_localtime) */
	return (rs >= 0) ? tl : rs ;
} /* end subroutine (mktimestr) */


