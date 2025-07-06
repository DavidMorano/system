/* snwcpycompact SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* counted-string copy while compacting white-space from the source */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10 David A-D- Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snwcpycompact

	Description:
	Similar to |snwcpy(3uc)| except that we copy the source
	to the destination while removing extra white-space from
	the source.

	Synopsis:
	int snwcpycompact(char *dbuf,int dlen,cchar *sp,int sl) noex

	Arguments:
	dbuf		result buffer
	dlen		length of supplied result buffer
	sp		source string
	sl		source string length

	Returns:
	>=0		resulting string length
	<0		error (system-return)

	Implemetation note:
	We could have used either |sbuf(3uc)| or |storebuf(3uc)|
	or some other subroutines of this ilk, but this subroutine
	was written at a time before resort to those interfaces was
	automatic. It is a little messy, but it works just fine as
	it is!

	See-also:
	snwcpy(3uc),
	snwcpylatin(3uc), 
	snwcpyopaque(3uc), 
	snwcpycompact(3uc), 
	snwcpyclean(3uc), 
	snwcpyhyphen(3uc), 

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<strmgr.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"snwcpyx.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snwcpycompact(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		dl = 0 ; /* return-value */
	if (dlen < 0) dlen = INT_MAX ;
	if (sl < 0) sl = lenstr(sp) ;
	if (strmgr m ; (rs = strmgr_start(&m,dbuf,dlen)) >= 0) {
	    int		cl ;
	    cchar	*cp ;
	    while ((cl = sfnext(sp,sl,&cp)) > 0) {
	        if (dl > 0) {
	            rs = strmgr_chr(&m,' ') ;
	            if (rs >= 0) dl += 1 ;
	        }
	        if (rs >= 0) {
	            rs = strmgr_str(&m,cp,cl) ;
	            if (rs >= 0) dl += cl ;
	        }
	        sl -= intconv((cp + cl) - sp) ;
	        sp = (cp + cl) ;
	        if (rs < 0) break ;
	    } /* end while (looping through string pieces) */
	    rs1 = strmgr_finish(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (strmgr) */
	dbuf[dl] = '\0' ;
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpycompact) */


