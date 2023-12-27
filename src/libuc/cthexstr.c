/* cthexstr SUPPORT */
/* lang=C20 */

/* subroutine to convert a value (as a counted string) to a HEX string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

	= 2017-08-15, David A­D­ Morano
	Rewrote to use the SBUF object.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cthexstring

	Description:
	This subroutine converts a counted array of bytes to a hexadecimal
	string.

	Synopsis:
	int cthexstring(char *dbuf,int dlen,int f,cchar *vp,int vl) noex

	Arguments:
	dbuf		result buffer
	dlen		length of result buffer
	f		flag specifying type of output
	vp		pointer to source buffer (value)
	vl		length of source in bytes

	Returns:
	<0		error (overflow)
	>=0		length of result bytes

	Example-output:
	+ for (!f)
		48656C6C6F20776F726C64210A
 	+ for (f)
		48 65 6C 6C 6F 20 77 6F 72 6C 64 21 0A

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<sbuf.h>
#include	<localmisc.h>

#include	"cthexstr.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int cthexstring(char *dbuf,int dlen,int f,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf && sp) {
	    SBUF	b ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = sbuf_start(&b,dbuf,dlen)) >= 0) {
	        const uchar	*vp = (const uchar *) sp ;
	        for (int i = 0 ; (rs >= 0) && (i < sl) ; i += 1) {
	            cint	ch = vp[i] ;
	            if (f && (i > 0)) {
		        rs = sbuf_char(&b,' ') ;
	            }
		    if (rs >= 0) {
			rs = sbuf_hexc(&b,ch) ;
		    }
	        } /* end for */
	        len = sbuf_finish(&b) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cthexstring) */


