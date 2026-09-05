/* paramopt_loadone SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load one parameter (similar to 'paramopt_loadu(3dam)' but special) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	paramopt_loadone

	Description:
	This is very similar to |paramopt_loadu(3dam)| but it allows
	for a special separator character between the key and the
	data values; the special separator character is ':'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<six.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"paramopt.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import paramopt_mag ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int paramopt_loadone(paramopt *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		idx = 0 ;
	if ((rs = paramopt_magic(op,sp)) >= 0) ylikely {
	    if (sl <= 0) sl = lenstr(sp) ;
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    if (int si ; (si = sibrk(sp,sl," \t=,:")) >= 0) ylikely {
	        cchar	*keyname ;
	        if (nulstr kn ; (rs = kn.start(sp,si,&keyname)) >= 0) ylikely {
	            sp += si ;
	            sl -= si ;
	            while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	                sp += 1 ;
	                sl -= 1 ;
	            } /* end while */
	            if ((sl > 0) && ((*sp == '=') || (*sp == ':'))) {
	                sp += 1 ;
	                sl -= 1 ;
	            } /* end if */
		    {
	                rs = paramopt_load(op,keyname,sp,sl) ;
		        idx = rs ;
		    }
	            rs1 = kn.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? idx : rs ;
} /* end subroutine (paramopt_loadone) */


