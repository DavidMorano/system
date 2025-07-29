/* cthexstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to convert a value (as a counted string) to a HEX string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-15, David A­D­ Morano
	Refactored to use the SBUF object.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cthexstr

	Description:
	This subroutine converts a counted array of bytes to a
	hexadecimal string.

	Synopsis:
	int cthexstr(char *dbuf,int dlen,int f,cchar *vp,int vl) noex

	Arguments:
	dbuf		result buffer
	dlen		length of result buffer
	f		flag specifying type of output
	vp		pointer to source buffer (value)
	vl		length of source in bytes

	Returns:
	>=0		length of result bytes
	<0		error (system-return)

	Example-output:
	+ for (!f)
		48656C6C6F20776F726C64210A
 	+ for (f)
		48 65 6C 6C 6F 20 77 6F 72 6C 64 21 0A

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sbuf.h>

#include	"cthexstr.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int cthexstr(char *dbuf,int dlen,int f,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf && sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sbuf b ; (rs = b.start(dbuf,dlen)) >= 0) {
	        const uchar	*up = (const uchar *) sp ;
	        for (int i = 0 ; (rs >= 0) && (i < sl) ; i += 1) {
	            cint	ch = up[i] ;
	            if (f && (i > 0)) {
		        rs = b.chr(' ') ;
	            }
		    if (rs >= 0) {
			rs = b.hexc(ch) ;
		    }
	        } /* end for */
	        len = b.finish ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cthexstr) */


