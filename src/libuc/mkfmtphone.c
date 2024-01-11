/* mkfmtphone */
/* lang=C20 */

/* similar to 'snwcpy(3dam)' but formatting a phone number */
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


*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<usystem.h>
#include	<ascii.h>
#include	<sbuf.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int mkfmtphone(char *dbuf,int dlen,cchar *pp,int pl) noex {
	int		dl = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl ;
	cchar		*sp ;
	if ((sl = sfshrink(pp,pl,&sp)) > 0) {
	    if (strnpbrk(sp,sl,"-()") == NULL) {
	        cint	tlen = sl ;
	        char	*tbuf ;
	        if ((rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
	            if ((rs = snwcpyopaque(tbuf,tlen,sp,sl)) >= 0) {
			SBUF	b ;
		        cint	tl = rs ;
			if ((rs = sbuf_start(&b,dbuf,dlen)) >= 0) {
			    if (tl > 10) {
				rs = sbuf_strw(&b,tbuf,(tl-10)) ;
			    }
		            if ((rs >= 0) && (tl >= 10)) {
				cchar	*tp = (tbuf+tl-10) ;
				sbuf_char(&b,CH_LPAREN) ;
				sbuf_strw(&b,tp,3) ;
				sbuf_char(&b,CH_RPAREN) ;
			    }
			    if ((rs >= 0) && (tl >= 7)) {
				cchar	*tp = (tbuf+tl-7) ;
				sbuf_strw(&b,tp,3) ;
				sbuf_char(&b,CH_MINUS) ;
			    }
			    if (rs >= 0) {
 			        if (tl >= 4) {
				    cchar	*tp = (tbuf+tl-4) ;
				    sbuf_strw(&b,tp,4) ;
			        } else {
				    sbuf_strw(&b,tbuf,tl) ;
				}
			    }
			    dl = sbuf_finish(&b) ;
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


