/* snwcpyshrink SUPPORT */
/* lang=C++20 */

/* copy the shrunken part of a source string */
/* version %I% last-modified %G% */

#define	CF_ONEPASS	1		/* try one-pass over the string */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

	= 2017-08-23, David A­D­ Morano
	I added the experimental one-pass version.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpyshrink

	Description:
	This subroutine constructs a destination string from a
	shrunken source string.

	Synopsis:
	int snwcpyshrink(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination buffer pointer
	dl		destination buffer length
	sp		source string pointer
	sl		source string length

	Returns:
	-		length of found string (in destination buffer)

	Notes:
	Q. Should we bother with ignoring all data after an NL character?
	A. Good question.  I do not know the answer.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<ascii.h>
#include	<sfx.h>
#include	<strmgr.h>
#include	<strn.h>
#include	<char.h>
#include	<localmisc.h>

#include	"snwcpyx.h"


/* local defines */


/* external subroutines */


/* exported variables */


/* exported subroutines */

#if	CF_ONEPASS
int snwcpyshrink(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*cp ;
	dbuf[0] = '\0' ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	    strmgr	d ;
	    if ((rs = strmgr_start(&d,dbuf,dlen)) >= 0) {
		while ((rs >= 0) && cl--) {
		    if (*cp == CH_NL) break ;
		    rs = strmgr_char(&d,*cp) ;
		    cp += 1 ;
		} /* end while */
		rs1 = strmgr_finish(&d) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (strmgr) */
	} /* end if (sfshrink) */
	return rs ;
}
/* end subroutine (snwcpyshrink) */
#else /* CF_ONEPASS */
int snwcpyshrink(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
	dbuf[0] = '\0' ;
	if (cchar *tp ; (tp = strnchr(sp,sl,CH_NL)) != nullptr) {
	    sl = (tp-sp) ;
	}
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	    rs = snwcpy(dbuf,dlen,cp,cl) ;
	}
	return rs ;
}
/* end subroutine (snwcpyshrink) */
#endif /* CF_ONEPASS */


