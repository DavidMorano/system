/* hdrctype SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* parse a mailmsg content-type into its parts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/****************************************************************************

  	Object:
	hdrctype

	Description:
	This object module parses a "content-type" header specification.
	The parsed results are broken into three types of items:
	the type, the sub-type, and parameters.

****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<strn.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"hdrctype.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int hdrctype_decode(hdrctype *op,cchar *hp,int hl) noex {
    	HDRCTYPE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && hp) {
	    int		cl ;
	    cchar	*cp ;
	    cchar	*tp ;
	    rs = memclear(hop) ;
	    if (hl < 0) hl = cstrlen(hp) ;
/* ignore any parameters */
	    if ((tp = strnchr(hp,hl,';')) != nullptr) {
	        hl = intconv((hp+hl) - tp) ;
	    }
/* parse the type and subtype */
	    if ((tp = strnchr(hp,hl,'/')) != nullptr) {
	        cchar	*sp = (tp +1) ;
	        cint	sl = intconv((hp + hl) - (tp + 1)) ;
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
		    op->sub.tp = cp ;
		    op->sub.tl = cl ;
	        }
	        hl = intconv(tp - hp) ;
	    }
	    if ((cl = sfshrink(hp,hl,&cp)) > 0) {
	        op->main.tp = cp ;
	        op->main.tl = cl ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdrctype_decode) */


