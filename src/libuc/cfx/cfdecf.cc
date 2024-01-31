/* cfdecf SUPPORT */
/* lang=C++20 */

/* convert a decimal digit string to its binary floating value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written adapted from assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cfdecf

	Description:
	Subroutines to convert decimal strings to binary floating
	values.

	Synopsis:

	Arguments:
	sp		source string
	sl		source string length
	rp		pointer to hold result

	Returns:
	>=0		OK
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstring>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<sfx.h>
#include	<ischarx.h>
#include	<char.h>
#include	<localmisc.h>		/* <- for |DIGBUFLEN| */

#include	"cfdecf.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int cfdecf(cchar *snp,int snl,double *rp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (snp && rp) {
	    int		sl ;
	    cchar	*sp{} ;
	    rs = SR_DOM ;
	    if ((sl = sfshrink(snp,snl,&sp)) > 0) {
	        cint	dlen = DIGBUFLEN ;
	        int	bl = sl ;
	        char	dbuf[DIGBUFLEN + 1] ;
	        cchar	*bp = sp ;
		rs = SR_OK ;
	        if ((sl >= 0) && sp[sl]) {
	            bp = dbuf ;
	            rs = snwcpy(dbuf,dlen,sp,sl) ;
	            bl = rs ;
	        }
	        if ((rs >= 0) && bl) {
	            cint	ch = MKCHAR(*bp) ;
	            if ((! isdigitlatin(ch)) && (ch != '.')) {
	                rs = SR_INVALID ;
	            }
	        }
	        if (rs >= 0) {
	            char	*ep{} ;
	            rs = uc_strtod(bp,&ep,rp) ;
		    len = (ep - bp) ;
	        }
	    } /* end if (sfshrink) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cfdecf) */


