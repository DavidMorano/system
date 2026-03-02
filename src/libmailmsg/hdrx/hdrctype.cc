/* hdrctype SUPPORT */
/* charset=ISO8859-1 */
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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<estrings.h>
#include	<strn.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"hdrctype.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int hdrctype_decode(hdrctype *op,cchar *hp,int µhl) noex {
    	HDRCTYPE	*hop = op ;
	int		rs = SR_FAULT ;
	if (int hl ; op && ((hl = getlenstr(hp,µhl)) >= 0)) {
	    int		cl ;
	    cchar	*cp ;
	    cchar	*tp ;
	    rs = memclear(hop) ;
	    /* ignore any parameters */
	    if ((tp = strnchr(hp,hl,';')) != nullptr) {
	        hl = intconv((hp + hl) - tp) ;
	    } /* end if (strnchr) */
	    /* parse the type and subtype */
	    if ((tp = strnchr(hp,hl,'/')) != nullptr) {
	        cchar	*sp = (tp +1) ;
	        cint	sl = intconv((hp + hl) - (tp + 1)) ;
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
		    op->sub.tp = cp ;
		    op->sub.tl = cl ;
	        }
	        hl = intconv(tp - hp) ;
	    } /* end if (strnchr) */
	    if ((cl = sfshrink(hp,hl,&cp)) > 0) {
	        op->main.tp = cp ;
	        op->main.tl = cl ;
	    }
	} /* end if (getlenstr) */
	return rs ;
}
/* end subroutine (hdrctype_decode) */


