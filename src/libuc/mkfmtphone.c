/* mkfmtphone SUPPORT */
/* lang=C++20 */

/* similar to |snwcpy(3dam)| but formatting a phone number */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-14, David A­D­ Morano
        This subroutine was originally rwritten.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfmtphone

	Description:
	We format a raw phone number to make it look more pretty.

	Synopsis:
	int mkfmtphone(char *dbuf,int dlen,cchar *pp,int pl) noex

	Arguments:
	dbuf		result buffer pointer
	dlen		result buffer length
	pp		source c-string pointer
	pl		source c-string length

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<usystem.h>
#include	<ascii.h>
#include	<sbuf.h>
#include	<strn.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int mkfmtphone(char *dbuf,int dlen,cchar *pp,int pl) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		dl = 0 ;
	int		sl ;
	cchar		*sp ;
	if ((sl = sfshrink(pp,pl,&sp)) > 0) {
	    if (strnpbrk(sp,sl,"-()") == NULL) {
	        cint	tlen = sl ;
	        char	*tbuf ;
	        if ((rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
	            if ((rs = snwcpyopaque(tbuf,tlen,sp,sl)) >= 0) {
			sbuf	b ;
		        cint	tl = rs ;
			if ((rs = b.start(dbuf,dlen)) >= 0) {
			    if (tl > 10) {
				rs = b.strw(tbuf,(tl-10)) ;
			    }
		            if ((rs >= 0) && (tl >= 10)) {
				cchar	*tp = (tbuf+tl-10) ;
				b.chr(CH_LPAREN) ;
				b.strw(tp,3) ;
				b.chr(CH_RPAREN) ;
			    }
			    if ((rs >= 0) && (tl >= 7)) {
				cchar	*tp = (tbuf+tl-7) ;
				b.strw(tp,3) ;
				b.chr(CH_MINUS) ;
			    }
			    if (rs >= 0) {
 			        if (tl >= 4) {
				    cchar	*tp = (tbuf+tl-4) ;
				    b.strw(tp,4) ;
			        } else {
				    b.strw(tbuf,tl) ;
				}
			    }
			    dl = b.finish ;
			    if (rs >= 0) rs = dl ;
			} /* end if (sbuf) */
	            } /* end if (sncpyopaque) */
	            rs1 = uc_free(tbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } else {
		rs = snwcpy(dbuf,dlen,sp,sl) ;
		dl = rs ;
	    }
	} /* end if (sfshrink) */
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (mkfmtphone) */


