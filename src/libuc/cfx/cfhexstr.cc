/* cfhexstr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* convert from a HEX string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was adapted from assembly.  The original
	assembly goes wa...ay back.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cfhexstr

	Description:
	This subroutine converts a string of HEX digits into a
	character string.  Every two hexadecimal digits are converted
	into one character.  Yes, this was originally in assembly
	language (and YES, was incredibly much faster).

	Synopsis:
	int cfhexstr(cchar *sbuf,int slen,uchar *dbuf) noex

	Arguments:
	sbuf		address of string to be converted
	slen		len of source address to convert
	dbuf		address of buffer to store result

	Outputs:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sfx.h>
#include	<digval.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"cfhexstr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int cfhexstr(cchar *sp,int sl,uchar *rp) noex {
	int		rs = SR_FAULT ;
	const uchar	*rbuf = (const uchar *) rp ;
	if (sp && rp) {
	    cchar	*cp ;
	    rs = SR_INVALID ;
	    if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	        while ((rs >= 0) && (cl >= 2) && cp[0]) {
	            cint	ch0 = mkchar(cp[0]) ;
	            cint	ch1 = mkchar(cp[1]) ;
	            if (ishexlatin(ch0) && ishexlatin(ch1)) {
		        int	v = 0 ;
	                v |= (digvalhex(ch0)<<4) ;
	                v |= (digvalhex(ch1)<<0) ;
	                *rp++ = uchar(v) ;
	            } else {
		        rs = SR_INVALID ;
	            }
	            cp += 2 ;
	            cl -= 2 ;
	        } /* end while */
	        if ((rs >= 0) && (cl > 0)) {
		    rs = SR_INVALID ;
	        }
	    } /* end if (got some) */
	} /* end if (non-null) */
	return (rs >= 0) ? (rp-rbuf) : rs ;
}
/* end subroutine (cfhexstr) */


