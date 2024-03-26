/* hdrctype SUPPORT */
/* lang=C++20 */

/* process the input messages and spool them up */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/****************************************************************************

	This object module parses a "content-type" header specification.
	The parsed results are broken into three types of items:
	the type, the sub-type, and parameters.

****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<strn.h>
#include	<sfx.h>
#include	<char.h>
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
	int		rs = SR_FAULT ;
	if (op && hp) {
	    int		cl ;
	    cchar	*cp ;
	    cchar	*tp ;
	    rs = memclear(op) ;
	    if (hl < 0) hl = strlen(hp) ;
/* ignore any parameters */
	    if ((tp = strnchr(hp,hl,';')) != nullptr) {
	        hl = ((hp+hl) - tp) ;
	    }
/* parse the type and subtype */
	    if ((tp = strnchr(hp,hl,'/')) != nullptr) {
	        cchar	*sp = (tp+1) ;
	        int	sl = ((hp+hl) - (tp+1)) ;
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
		    op->sub.tp = cp ;
		    op->sub.tl = cl ;
	        }
	        hl = (tp-hp) ;
	    }
	    if ((cl = sfshrink(hp,hl,&cp)) > 0) {
	        op->main.tp = cp ;
	        op->main.tl = cl ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdrctype_decode) */


